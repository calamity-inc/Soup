#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include <vector>

#if SOUP_WINDOWS
#include <WinSock2.h>
#else
#include <poll.h>
#endif

#include "AtomicStack.hpp"
#include "UniquePtr.hpp"
#include "Worker.hpp"

namespace soup
{
	class Scheduler
	{
	protected:
		std::vector<UniquePtr<Worker>> workers{};
		AtomicStack<UniquePtr<Worker>> pending_workers{};

	public:
		using on_work_done_t = void(*)(Worker&, Scheduler&);
		using on_connection_lost_t = void(*)(Socket&, Scheduler&);
		using on_exception_t = void(*)(Worker&, const std::exception&, Scheduler&);

		on_work_done_t on_work_done = nullptr;
		on_connection_lost_t on_connection_lost = nullptr;
		on_exception_t on_exception = nullptr;

		virtual ~Scheduler() = default;

		virtual Worker& addWorker(UniquePtr<Worker>&& w) noexcept;

		Socket& addSocket() noexcept;
		Socket& addSocket(UniquePtr<Socket>&& sock) noexcept;

		template <typename T, SOUP_RESTRICT(std::is_same_v<T, Socket>)>
		T& addSocket(T&& sock) noexcept
		{
			return addSocket(make_unique<Socket>(std::move(sock)));
		}

		template <typename T, typename...Args>
		T& add(Args&&...args)
		{
			return static_cast<T&>(addWorker(make_unique<T>(std::forward<Args>(args)...)));
		}

		void run();
		void runFor(unsigned int ms);
	protected:
		void tick(std::vector<pollfd>& pollfds, bool& not_just_sockets);
		void tickWorker(std::vector<pollfd>& pollfds, bool& not_just_sockets, Worker& w);
		void yieldBusyspin(std::vector<pollfd>& pollfds);
		void yieldKernel(std::vector<pollfd>& pollfds);
		int poll(std::vector<pollfd>& pollfds, int timeout);
		void processPollResults(std::vector<pollfd>& pollfds);
		void fireHoldupCallback(Worker& w);
		void onConnectionLoss(std::vector<UniquePtr<Worker>>::iterator& workers_i);
	};
}

#endif
