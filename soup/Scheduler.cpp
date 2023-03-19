#include "Scheduler.hpp"

#if !SOUP_WASM

#include <thread>

#include "Promise.hpp"
#include "ReuseTag.hpp"
#include "Socket.hpp"
#include "Task.hpp"
#include "time.hpp"

#define LOG_TICK_DUR false

#if LOG_TICK_DUR
#include "format.hpp"
#include "log.hpp"
#include "Stopwatch.hpp"
#endif

namespace soup
{
	Worker& Scheduler::addWorker(UniquePtr<Worker>&& w) noexcept
	{
		return *pending_workers.emplace_front(std::move(w))->data;
	}

	Socket& Scheduler::addSocket() noexcept
	{
		return addSocket(soup::make_unique<Socket>());
	}

	Socket& Scheduler::addSocket(UniquePtr<Socket>&& sock) noexcept
	{
#if SOUP_LINUX
		sock->setNonBlocking();
#endif
		return static_cast<Socket&>(addWorker(std::move(sock)));
	}

	void Scheduler::run()
	{
		while (!workers.empty() || !pending_workers.empty())
		{
			bool not_just_sockets = false;
			std::vector<pollfd> pollfds{};
#if LOG_TICK_DUR
			Stopwatch t;
#endif
			tick(pollfds, not_just_sockets);
#if LOG_TICK_DUR
			t.stop();
			logWriteLine(format("Tick took {} ms", t.getMs()));
#endif
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
		// Schedule in pending workers
		{
			auto num_pending_workers = pending_workers.size();
			SOUP_IF_UNLIKELY (num_pending_workers != 0)
			{
				workers.reserve(workers.size() + num_pending_workers);
				do
				{
					auto worker = pending_workers.pop_front();
					workers.emplace_back(std::move(*worker));
				} while (--num_pending_workers);
			}
		}

		// Process workers
		for (auto i = workers.begin(); i != workers.end(); )
		{
			if ((*i)->type == WORKER_TYPE_SOCKET)
			{
				SOUP_IF_UNLIKELY (reinterpret_cast<Socket*>(i->get())->fd == -1)
				{
					processClosedSocket(*reinterpret_cast<Socket*>(i->get()));
					++i;
					continue;
				}
			}
			SOUP_IF_UNLIKELY ((*i)->holdup_type == Worker::NONE)
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
				0
			});

			if (w.holdup_type == Worker::IDLE)
			{
				fireHoldupCallback(w);
			}
			else //if (w.holdup_type == Worker::PROMISE)
			{
				if (!reinterpret_cast<PromiseBase*>(w.holdup_data)->isPending())
				{
					fireHoldupCallback(w);
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

	void Scheduler::processPollResults(const std::vector<pollfd>& pollfds)
	{
		for (auto i = pollfds.begin(); i != pollfds.end(); ++i)
		{
			if (i->revents != 0
				&& i->fd != -1
				)
			{
				auto workers_i = workers.begin() + (i - pollfds.begin());
				if (i->revents & ~POLLIN)
				{
					reinterpret_cast<Socket*>(workers_i->get())->remote_closed = true;
					processClosedSocket(*reinterpret_cast<Socket*>(workers_i->get()));
				}
				else
				{
					fireHoldupCallback(**workers_i);
				}
			}
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
			w.holdup_type = Worker::NONE;
		}
	}

	void Scheduler::processClosedSocket(Socket& s)
	{
		if (on_connection_lost)
		{
			if (!s.dispatched_connection_lost)
			{
				s.dispatched_connection_lost = true;
				on_connection_lost(s, *this);
			}
		}

		if (s.holdup_type == Worker::SOCKET)
		{
			if (s.callback_recv_on_close
				|| s.transport_hasData()
				)
			{
				// Socket still has stuff to do...
				fireHoldupCallback(s);
			}
			else if (s.hasRefs())
			{
				// There are tasks referencing this socket...
			}
			else
			{
				// No excuses, slate for execution.
				s.holdup_type = Worker::NONE;
			}
		}
	}

	Socket* Scheduler::findReusableSocketForHost(const std::string& host)
	{
		for (const auto& w : workers)
		{
			if (w->type == WORKER_TYPE_SOCKET
				&& static_cast<Socket*>(w.get())->custom_data.isStructInMap(ReuseTag)
				&& static_cast<Socket*>(w.get())->custom_data.getStructFromMap(ReuseTag).host == host
				)
			{
				return static_cast<Socket*>(w.get());
			}
		}
		return nullptr;
	}

	void Scheduler::closeReusableSockets()
	{
		for (const auto& w : workers)
		{
			if (w->type == WORKER_TYPE_SOCKET
				&& static_cast<Socket*>(w.get())->custom_data.isStructInMap(ReuseTag)
				&& !static_cast<Socket*>(w.get())->custom_data.getStructFromMap(ReuseTag).is_busy
				)
			{
				static_cast<Socket*>(w.get())->close();
			}
		}
	}
}

#endif
