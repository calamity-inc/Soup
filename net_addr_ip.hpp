#pragma once

#include <cstdint>
#include <string>

#include "platform.hpp"

#if SOUP_PLATFORM_WINDOWS
#include <Ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

namespace soup
{
	struct net_addr_ip
	{
		in6_addr data;

		[[nodiscard]] bool isV4() const noexcept
		{
			return IN6_IS_ADDR_V4MAPPED(&data);
		}

		[[nodiscard]] std::string toString() const noexcept
		{
			if(isV4())
			{
				return toString4();
			}
			return toString6();
		}

		[[nodiscard]] std::string toString4() const noexcept
		{
			char buf[INET_ADDRSTRLEN] = { '\0' };
			inet_ntop(AF_INET, reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(&data)+12), buf, INET_ADDRSTRLEN);
			return buf;
		}

		[[nodiscard]] std::string toString6() const noexcept
		{
			char buf[INET6_ADDRSTRLEN] = { '\0' };
			inet_ntop(AF_INET6, &data, buf, INET6_ADDRSTRLEN);
			return buf;
		}
	};
}
