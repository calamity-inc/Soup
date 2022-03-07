#pragma once

#include <vector>

#include "base.hpp"

#if SOUP_LINUX
#include <poll.h>
#endif

#include "socket.hpp"

namespace soup
{
	class socket_mgr
	{
	public:
		std::vector<pollfd> pollfds{};
		std::vector<socket> sockets{};

		using on_accept_t = void(*)(socket&, uint16_t port);
		using on_disconnect_t = void(*)(socket&);

		on_accept_t on_accept = nullptr;
		on_disconnect_t on_disconnect = nullptr;

		socket& addSocket(socket&& sock) noexcept; // socket must have connection and fd can't change while managed

		bool bind(uint16_t port) noexcept;
	protected:
		bool processAccept(socket&& sock, uint16_t port);

	public:
		void run();

		void fireDisconnect(std::vector<socket>::iterator sockets_i);

		[[nodiscard]] std::vector<pollfd>::iterator socketToPollfd(std::vector<socket>::iterator sockets_i);
		[[nodiscard]] std::vector<socket>::iterator pollfdToSocket(std::vector<pollfd>::iterator pollfds_i);
	};
}
