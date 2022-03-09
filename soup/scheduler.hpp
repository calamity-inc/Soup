#pragma once

#include "base.hpp"

#include <memory>
#include <vector>

#if SOUP_WINDOWS
#include <WinSock2.h>
#else
#include <poll.h>
#endif

#include "worker.hpp"

namespace soup
{
	class scheduler
	{
	public:
		std::vector<std::unique_ptr<worker>> workers{};

		using on_work_done_t = void(*)(worker&);
		using on_connection_lost_t = void(*)(socket&);

		on_work_done_t on_work_done = nullptr;
		on_connection_lost_t on_connection_lost = nullptr;

		socket& addSocket(socket&& sock) noexcept;

		void run();
	protected:
		int poll(std::vector<pollfd>& pollfds, int timeout = -1);
		void processPollResults(std::vector<pollfd>& pollfds);
	};
}
