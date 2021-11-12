#pragma once

#include "platform.hpp"

#if SOUP_PLATFORM_WINDOWS
#include <Winsock2.h> // closesocket, WSAGetLastError
#else
#include <string.h> // strerror
#include <unistd.h> // close
#endif

namespace soup
{
	struct net_socket
	{
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
			if(fd != -1)
			{
#if SOUP_PLATFORM_WINDOWS
				closesocket(fd);
#else
				close(fd);
#endif
			}
		}

		[[nodiscard]] bool isValid() const noexcept
		{
			return fd != -1;
		}
	};
}
