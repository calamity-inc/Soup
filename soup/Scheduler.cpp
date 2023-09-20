#include "Scheduler.hpp"

#if !SOUP_WASM

#include <thread>

#include "log.hpp"
#include "Promise.hpp"
#include "ReuseTag.hpp"
#include "Socket.hpp"
#include "Task.hpp"
#include "time.hpp"

#define LOG_TICK_DUR false

#if LOG_TICK_DUR
#include "format.hpp"
#include "Stopwatch.hpp"
#endif

namespace soup
{
	static thread_local Scheduler* this_thread_running_scheduler = nullptr;

	SharedPtr<Worker> Scheduler::addWorker(SharedPtr<Worker>&& w) noexcept
	{
		return pending_workers.emplace_front(std::move(w))->data;
	}

	SharedPtr<Socket> Scheduler::addSocket() noexcept
	{
		return addSocket(soup::make_shared<Socket>());
	}

	SharedPtr<Socket> Scheduler::addSocket(SharedPtr<Socket>&& sock) noexcept
	{
#if SOUP_LINUX
		sock->setNonBlocking();
#endif
		return addWorker(std::move(sock));
	}

	void Scheduler::run()
	{
		SOUP_ASSERT(this_thread_running_scheduler == nullptr);
		this_thread_running_scheduler = this;
		while (shouldKeepRunning())
		{
			std::vector<pollfd> pollfds{};
			uint8_t workload_flags = 0;
#if LOG_TICK_DUR
			Stopwatch t;
#endif
			tick(pollfds, workload_flags);
#if LOG_TICK_DUR
			t.stop();
			logWriteLine(format("Tick took {} ms", t.getMs()));
#endif
			if (workload_flags & NOT_JUST_SOCKETS)
			{
				yieldBusyspin(pollfds, workload_flags);
			}
			else
			{
				yieldKernel(pollfds);
			}
		}
		this_thread_running_scheduler = nullptr;
	}

	void Scheduler::runFor(unsigned int ms)
	{
		SOUP_ASSERT(this_thread_running_scheduler == nullptr);
		this_thread_running_scheduler = this;
		time_t deadline = time::millis() + ms;
		while (shouldKeepRunning())
		{
			std::vector<pollfd> pollfds{};
			uint8_t workload_flags = 0;
			tick(pollfds, workload_flags);
			yieldBusyspin(pollfds, workload_flags);
			if (time::millis() > deadline)
			{
				break;
			}
		}
		this_thread_running_scheduler = nullptr;
	}

	bool Scheduler::shouldKeepRunning() const
	{
		return workers.size() != passive_workers || !pending_workers.empty();
	}

	void Scheduler::tick()
	{
		SOUP_ASSERT(this_thread_running_scheduler == nullptr);
		this_thread_running_scheduler = this;

		std::vector<pollfd> pollfds{};
		uint8_t workload_flags = 0;
		tick(pollfds, workload_flags);
		if (poll(pollfds, 0) > 0)
		{
			processPollResults(pollfds);
		}

		this_thread_running_scheduler = nullptr;
	}

	void Scheduler::tick(std::vector<pollfd>& pollfds, uint8_t& workload_flags)
	{
		// Schedule in pending workers
		{
			auto num_pending_workers = pending_workers.size();
			SOUP_IF_UNLIKELY (num_pending_workers != 0)
			{
				workers.reserve(workers.size() + num_pending_workers);
				do
				{
					auto worker = pending_workers.pop_back();
					workers.emplace_back(std::move(*worker));
				} while (--num_pending_workers);
			}
		}

		// Process workers
		for (auto i = workers.begin(); i != workers.end(); )
		{
			if ((*i)->type == WORKER_TYPE_SOCKET)
			{
				SOUP_IF_UNLIKELY (static_cast<Socket*>(i->get())->fd == -1)
				{
					processClosedSocket(*static_cast<Socket*>(i->get()));
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
			tickWorker(pollfds, workload_flags, **i);
			++i;
		}
	}

	void Scheduler::tickWorker(std::vector<pollfd>& pollfds, uint8_t& workload_flags, Worker& w)
	{
		if (w.holdup_type == Worker::SOCKET)
		{
			pollfds.emplace_back(pollfd{
				static_cast<Socket&>(w).fd,
				POLLIN
			});
		}
		else
		{
			pollfds.emplace_back(pollfd{
				(Socket::fd_t)-1,
				0
			});

			workload_flags |= NOT_JUST_SOCKETS;

			if (w.holdup_type == Worker::IDLE)
			{
				if (w.type == WORKER_TYPE_TASK
					&& static_cast<Task&>(w).benefitsFromHighFrequency()
					)
				{
					workload_flags |= HAS_HIGH_FREQUENCY_TASKS;
				}
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

	void Scheduler::yieldBusyspin(std::vector<pollfd>& pollfds, uint8_t workload_flags)
	{
		if (poll(pollfds, 0) > 0)
		{
			processPollResults(pollfds);
		}
		if (!(workload_flags & HAS_HIGH_FREQUENCY_TASKS))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	void Scheduler::yieldKernel(std::vector<pollfd>& pollfds)
	{
		// Why timeout=1?
		// - On Linux, poll does not detect closed sockets, even if shutdown is used.
		// - If a scheduler that is only waiting on sockets has addWorker called on it, we don't want an insane delay until that worker starts.
		int timeout = 1;
#if SOUP_WINDOWS
		if (add_worker_can_wait_forever_for_all_i_care)
		{
			timeout = -1;
		}
#endif
		if (poll(pollfds, timeout) > 0)
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
#ifdef _DEBUG
		w.fireHoldupCallback();
#else
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
#endif
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
			else
			{
				// No excuses, slate for execution.
				s.holdup_type = Worker::NONE;
			}
		}
	}

	Scheduler* Scheduler::get()
	{
		return this_thread_running_scheduler;
	}

	size_t Scheduler::getNumWorkers() const
	{
		return workers.size();
	}

	size_t Scheduler::getNumWorkersOfType(uint8_t type) const
	{
		size_t res = 0;
		for (const auto& w : workers)
		{
			if (w->type == type)
			{
				++res;
			}
		}
		return res;
	}

	size_t Scheduler::getNumSockets() const
	{
		return getNumWorkersOfType(WORKER_TYPE_SOCKET);
	}

	SharedPtr<Socket> Scheduler::getShared(const Worker& w) const
	{
		for (const auto& spW : workers)
		{
			if (spW.get() == &w)
			{
				return spW;
			}
		}
		return {};
	}

	SharedPtr<Socket> Scheduler::findReusableSocketForHost(const std::string& host)
	{
		for (const auto& w : workers)
		{
			if (w->type == WORKER_TYPE_SOCKET
				&& static_cast<Socket*>(w.get())->custom_data.isStructInMap(ReuseTag)
				&& static_cast<Socket*>(w.get())->custom_data.getStructFromMapConst(ReuseTag).host == host
				)
			{
				return w;
			}
		}
		return {};
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

	void Scheduler::on_exception_log(Worker& w, const std::exception& e, Scheduler&)
	{
		std::string msg = "Exception while processing ";
		msg.append(w.toString());
		msg.append(": ");
		msg.append(e.what());
		logWriteLine(std::move(msg));
	}
}

#endif
