#include "Scheduler.hpp"

#if !SOUP_WASM

#include <thread>

#include "Promise.hpp"
#include "Socket.hpp"
#include "Task.hpp"
#include "time.hpp"

namespace soup
{
	Socket& Scheduler::addSocket() noexcept
	{
		return addSocket(soup::make_unique<Socket>());
	}

	Socket& Scheduler::addSocket(UniquePtr<Socket>&& sock) noexcept
	{
#if SOUP_LINUX
		sock->setNonBlocking();
#endif
		return *reinterpret_cast<Socket*>(workers.emplace_back(std::move(sock)).get());
	}

	void Scheduler::run()
	{
		while (!workers.empty())
		{
			bool not_just_sockets = false;
			std::vector<pollfd> pollfds{};
			tick(pollfds, not_just_sockets);
			if (not_just_sockets)
			{
				yieldBusyspin(pollfds);
			}
			else
			{
				yieldKernel(pollfds);
			}
		}
	}

	void Scheduler::runFor(unsigned int ms)
	{
		time_t deadline = time::millis() + ms;
		while (!workers.empty())
		{
			bool not_just_sockets = false;
			std::vector<pollfd> pollfds{};
			tick(pollfds, not_just_sockets);
			yieldBusyspin(pollfds);
			if (time::millis() > deadline)
			{
				break;
			}
		}
	}

	void Scheduler::tick(std::vector<pollfd>& pollfds, bool& not_just_sockets)
	{
		for (auto i = workers.begin(); i != workers.end(); )
		{
			if ((*i)->type == WORKER_TYPE_SOCKET
				&& reinterpret_cast<Socket*>(i->get())->fd == -1
				)
			{
				onConnectionLoss(i);
				continue;
			}
			if ((*i)->holdup_type == Worker::NONE)
			{
				if (on_work_done)
				{
					on_work_done(*i->get(), *this);
				}
				i = workers.erase(i);
				continue;
			}
			tickWorker(pollfds, not_just_sockets, **i);
			++i;
		}
	}

	void Scheduler::tickWorker(std::vector<pollfd>& pollfds, bool& not_just_sockets, Worker& w)
	{
		if (w.holdup_type == Worker::SOCKET)
		{
			pollfds.emplace_back(pollfd{
				reinterpret_cast<Socket&>(w).fd,
				POLLIN
			});
		}
		else
		{
			not_just_sockets = true;
			pollfds.emplace_back(pollfd{
				(Socket::fd_t)-1,
				POLLIN
			});

			if (w.holdup_type == Worker::IDLE)
			{
				fireHoldupCallback(w);
			}
			else if (w.holdup_type == Worker::PROMISE)
			{
				if (!reinterpret_cast<PromiseBase*>(w.holdup_data)->isPending())
				{
					fireHoldupCallback(w);
				}
			}
			else //if (w.holdup_type == Worker::TASK)
			{
				Task& subtask = *w.holdup_callback.cap.get<UniquePtr<Task>>();
				if (subtask.holdup_type == Worker::NONE)
				{
					w.holdup_type = Worker::IDLE;
					fireHoldupCallback(w);
					w.holdup_callback.cap.reset();
				}
				else
				{
					tickWorker(pollfds, not_just_sockets, subtask);
				}
			}
		}
	}

	void Scheduler::yieldBusyspin(std::vector<pollfd>& pollfds)
	{
		if (poll(pollfds, 0) > 0)
		{
			processPollResults(pollfds);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	void Scheduler::yieldKernel(std::vector<pollfd>& pollfds)
	{
#if SOUP_LINUX
		if (poll(pollfds, 1) > 0) // On Linux, poll does not detect closed sockets, even if shutdown is used.
#else
		if (poll(pollfds, -1) > 0)
#endif
		{
			processPollResults(pollfds);
		}
	}

	int Scheduler::poll(std::vector<pollfd>& pollfds, int timeout)
	{
#if SOUP_WINDOWS
		return ::WSAPoll(pollfds.data(), pollfds.size(), timeout);
#else
		return ::poll(pollfds.data(), pollfds.size(), timeout);
#endif
	}

	void Scheduler::processPollResults(std::vector<pollfd>& pollfds)
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
					reinterpret_cast<Socket*>(workers_i->get())->remote_closed = true;
					if (!reinterpret_cast<Socket*>(workers_i->get())->transport_hasData()
						|| workers_i->get()->holdup_type != Worker::SOCKET
						)
					{
						onConnectionLoss(workers_i);
						i = pollfds.erase(i);
						continue;
					}
				}
				fireHoldupCallback(**workers_i);
			}
			++i;
		}
	}

	void Scheduler::fireHoldupCallback(Worker& w)
	{
		try
		{
			w.fireHoldupCallback();
		}
		catch (const std::exception& e)
		{
			if (on_exception)
			{
				on_exception(w, e, *this);
			}
		}
	}

	void Scheduler::onConnectionLoss(std::vector<UniquePtr<Worker>>::iterator& workers_i)
	{
		if (on_connection_lost)
		{
			on_connection_lost(*reinterpret_cast<Socket*>(workers_i->get()), *this);
		}
		workers_i = workers.erase(workers_i);
	}
}

#endif
