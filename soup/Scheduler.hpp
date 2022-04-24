#pragma once

#include <vector>

#include "base.hpp"

#if SOUP_WINDOWS
#include <WinSock2.h>
#else
#include <poll.h>
#endif

#include "UniquePtr.hpp"
#include "Worker.hpp"

namespace soup
{
	class Scheduler
	{
	public:
		std::vector<UniquePtr<Worker>> workers{};

		using on_work_done_t = void(*)(Worker&);
		using on_connection_lost_t = void(*)(Socket&);
		using on_exception_t = void(*)(Worker&, const std::exception&);

		on_work_done_t on_work_done = nullptr;
		on_connection_lost_t on_connection_lost = nullptr;
		on_exception_t on_exception = nullptr;

		Socket& addSocket(UniquePtr<Socket>&& sock) noexcept;
		Socket& addSocket(Socket&& sock) noexcept;

		void run();
	protected:
		int poll(std::vector<pollfd>& pollfds, int timeout = -1);
		void processPollResults(std::vector<pollfd>& pollfds);
		void fireHoldupCallback(Worker& w);
	};
}
