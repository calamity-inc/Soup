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

		net_addr_ip() noexcept = default;

		explicit net_addr_ip(const std::string& str)
		{
			if (str.find('.') == std::string::npos)
			{
				inet_pton(AF_INET6, str.data(), &data);
			}
			else
			{
				data.s6_words[0] = 0;
				data.s6_words[1] = 0;
				data.s6_words[2] = 0;
				data.s6_words[3] = 0;
				data.s6_words[4] = 0;
				data.s6_words[5] = 0xffff;
				inet_pton(AF_INET, str.data(), reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(&data) + 12));
			}
		}

		explicit net_addr_ip(const uint8_t bytes[16]) noexcept
		{
			memcpy(&data.s6_bytes, bytes, 16);
		}

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
