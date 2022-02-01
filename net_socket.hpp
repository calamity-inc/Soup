#pragma once

#include "platform.hpp"

#include "net_addr_socket.hpp"

#if SOUP_PLATFORM_WINDOWS
#pragma comment(lib, "Ws2_32.lib")
#include <Winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // close
#endif

namespace soup
{
	class net_socket
	{
	public:
#if SOUP_PLATFORM_WINDOWS
		inline static size_t wsa_consumers = 0;
#endif

		int fd = -1;

		constexpr net_socket() noexcept = default;

		net_socket(const net_socket&) = delete;

		net_socket(net_socket&& b) noexcept
			: fd(b.fd)
		{
			b.fd = -1;
		}

		void operator =(const net_socket&) = delete;

		void operator =(net_socket&& b) noexcept
		{
			fd = b.fd;
			b.fd = -1;
		}

		~net_socket() noexcept
		{
			release();
		}

		void release()
		{
			if (fd != -1)
			{
#if SOUP_PLATFORM_WINDOWS
				closesocket(fd);
#else
				close(fd);
#endif
				fd = -1;
			}
		}

		[[nodiscard]] bool isValid() const noexcept
		{
			return fd != -1;
		}

	protected:
		void preinit()
		{
#if SOUP_PLATFORM_WINDOWS
			if (wsa_consumers++ == 0)
			{
				WSADATA wsaData;
				WORD wVersionRequested = MAKEWORD(2, 2);
				WSAStartup(wVersionRequested, &wsaData);
			}
#endif
			release();
		}

	public:
		bool init(const net_addr_socket& addr_desc)
		{
			preinit();
			fd = socket(AF_INET6, SOCK_STREAM, 0);
			if (fd == -1)
			{
				return false;
			}
			sockaddr_in6 addr{};
			addr.sin6_family = AF_INET6;
			memcpy(&addr.sin6_addr, &addr_desc.addr.data, sizeof(in6_addr));
			addr.sin6_port = htons(addr_desc.port);
			if (::connect(fd, (sockaddr*)&addr, sizeof(addr)) == -1)
			{
				return false;
			}
			return true;
		}
	};
}
