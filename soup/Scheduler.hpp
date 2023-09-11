#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include <string>
#include <vector>

#if SOUP_WINDOWS
#include <WinSock2.h>
#else
#include <poll.h>
#endif

#include "AtomicDeque.hpp"
#include "SharedPtr.hpp"
#include "Worker.hpp"

namespace soup
{
	// The only thing you are allowed to do from other threads is add a worker since that can be done atomically.
	// All other operations are subject to race conditions.
	class Scheduler
	{
	public:
		std::vector<SharedPtr<Worker>> workers{};
		AtomicDeque<SharedPtr<Worker>> pending_workers{};
		size_t passive_workers = 0;
		bool dont_make_reusable_sockets = false;
#if SOUP_WINDOWS
		bool add_worker_can_wait_forever_for_all_i_care = false;
#endif

		using on_work_done_t = void(*)(Worker&, Scheduler&);
		using on_connection_lost_t = void(*)(Socket&, Scheduler&);
		using on_exception_t = void(*)(Worker&, const std::exception&, Scheduler&);

		on_work_done_t on_work_done = nullptr; // This will always be called before a worker is deleted.
		on_connection_lost_t on_connection_lost = nullptr; // Check remote_closed for which side caused connection to be lost.
		on_exception_t on_exception = &on_exception_log;

		virtual ~Scheduler() = default;

		virtual SharedPtr<Worker> addWorker(SharedPtr<Worker>&& w) noexcept;

		SharedPtr<Socket> addSocket() noexcept;
		SharedPtr<Socket> addSocket(SharedPtr<Socket>&& sock) noexcept;

		template <typename T, SOUP_RESTRICT(std::is_same_v<T, Socket>)>
		SharedPtr<T> addSocket(T&& sock) noexcept
		{
			return addSocket(soup::make_shared<Socket>(std::move(sock)));
		}

		template <typename T, typename...Args>
		SharedPtr<T> add(Args&&...args)
		{
			return addWorker(soup::make_shared<T>(std::forward<Args>(args)...));
		}

		void run();
		void runFor(unsigned int ms);
		[[nodiscard]] bool shouldKeepRunning() const;
		void tick();
	protected:
		void tick(std::vector<pollfd>& pollfds, bool& not_just_sockets);
		void tickWorker(std::vector<pollfd>& pollfds, bool& not_just_sockets, Worker& w);
		void yieldBusyspin(std::vector<pollfd>& pollfds);
		void yieldKernel(std::vector<pollfd>& pollfds);
		int poll(std::vector<pollfd>& pollfds, int timeout);
		void processPollResults(const std::vector<pollfd>& pollfds);
		void fireHoldupCallback(Worker& w);
		void processClosedSocket(Socket& s);

	public:
		[[nodiscard]] static Scheduler* get();

		[[nodiscard]] size_t getNumWorkers() const;
		[[nodiscard]] size_t getNumWorkersOfType(uint8_t type) const;
		[[nodiscard]] size_t getNumSockets() const;

		[[nodiscard]] SharedPtr<Socket> getShared(const Worker& w) const;
		[[nodiscard]] SharedPtr<Socket> findReusableSocketForHost(const std::string& host);
		void closeReusableSockets();

		static void on_exception_log(Worker& w, const std::exception& e, Scheduler&);
	};
}

#endif
