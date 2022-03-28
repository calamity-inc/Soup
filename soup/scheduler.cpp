#include "scheduler.hpp"

#include <thread>

#include "promise.hpp"
#include "socket.hpp"

namespace soup
{
	socket& scheduler::addSocket(socket&& sock) noexcept
	{
		return *reinterpret_cast<socket*>(workers.emplace_back(std::make_unique<socket>(std::move(sock))).get());
	}

	void scheduler::run()
	{
		while (!workers.empty())
		{
			bool not_just_sockets = false;
			std::vector<pollfd> pollfds{};
			for (auto i = workers.begin(); i != workers.end(); )
			{
				if ((*i)->holdup_type == worker::NONE)
				{
					if (on_work_done)
					{
						on_work_done(*i->get());
					}
					i = workers.erase(i);
					continue;
				}
				if ((*i)->holdup_type == worker::SOCKET)
				{
					pollfds.emplace_back(pollfd{
						reinterpret_cast<socket*>(i->get())->fd,
						POLLIN
					});
				}
				else
				{
					not_just_sockets = true;
					pollfds.emplace_back(pollfd{
						(socket::fd_t)-1,
						POLLIN
					});

					//if ((*i)->holdup_type == worker::PROMISE)
					{
						if (!reinterpret_cast<promise_base*>((*i)->holdup_data)->isPending())
						{
							fireHoldupCallback(**i);
						}
					}
				}
				++i;
			}
			if (not_just_sockets)
			{
				if (poll(pollfds, 0) > 0)
				{
					processPollResults(pollfds);
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			else
			{
				if (poll(pollfds) > 0)
				{
					processPollResults(pollfds);
				}
			}
		}
	}

	int scheduler::poll(std::vector<pollfd>& pollfds, int timeout)
	{
#if SOUP_WINDOWS
		return ::WSAPoll(pollfds.data(), pollfds.size(), timeout);
#else
		return ::poll(pollfds.data(), pollfds.size(), timeout);
#endif
	}

	void scheduler::processPollResults(std::vector<pollfd>& pollfds)
	{
		for (auto i = pollfds.begin(); i != pollfds.end(); )
		{
			if (i->revents != 0
				&& i->fd != -1
				)
			{
				auto workers_i = workers.begin() + (i - pollfds.begin());
				if (i->revents & ~POLLIN)
				{
					reinterpret_cast<socket*>(workers_i->get())->remote_closed = true;
					if (!reinterpret_cast<socket*>(workers_i->get())->transport_hasData())
					{
						if (on_connection_lost)
						{
							on_connection_lost(*reinterpret_cast<socket*>(workers_i->get()));
						}
						workers.erase(workers_i);
						i = pollfds.erase(i);
						continue;
					}
				}
				fireHoldupCallback(**workers_i);
			}
			++i;
		}
	}

	void scheduler::fireHoldupCallback(worker& w)
	{
		try
		{
			w.fireHoldupCallback();
		}
		catch (const std::exception& e)
		{
			if (on_exception)
			{
				on_exception(w, e);
			}
		}
	}
}
