#pragma once

#include <cstdint>
#include <cstring> // memcpy
#include <string>

#include "base.hpp"

#if SOUP_WINDOWS
#pragma comment(lib, "Ws2_32.lib")
#include <Ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

namespace soup
{
	class IpAddr
	{
	public:
		in6_addr data;

		IpAddr() noexcept = default;

		IpAddr(const char* str)
		{
			if (strstr(str, ".") == nullptr)
			{
				inet_pton(AF_INET6, str, &data);
			}
			else
			{
				setV4();
				inet_pton(AF_INET, str, reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(&data) + 12));
			}
		}

		IpAddr(const std::string& str)
		{
			if (str.find('.') == std::string::npos)
			{
				inet_pton(AF_INET6, str.data(), &data);
			}
			else
			{
				setV4();
				inet_pton(AF_INET, str.data(), reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(&data) + 12));
			}
		}

		IpAddr(const IpAddr& b) noexcept
		{
			memcpy(&data, &b.data, sizeof(data));
		}

		explicit IpAddr(const uint8_t* bytes) noexcept
		{
			memcpy(&data, bytes, sizeof(data));
		}

		explicit IpAddr(const uint32_t ipv4) noexcept
		{
			operator =(ipv4);
		}

		bool fromString(const std::string& str);

		void operator = (const uint32_t ipv4) noexcept
		{
			setV4();
			*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(&data) + 12) = ipv4;
		}

		[[nodiscard]] bool operator ==(const IpAddr& b) const noexcept
		{
			return memcmp(reinterpret_cast<const void*>(&data), reinterpret_cast<const void*>(&b.data), sizeof(data)) == 0;
		}

		[[nodiscard]] bool operator !=(const IpAddr& b) const noexcept
		{
			return memcmp(reinterpret_cast<const void*>(&data), reinterpret_cast<const void*>(&b.data), sizeof(data)) != 0;
		}

		[[nodiscard]] bool operator <(const IpAddr& b) const noexcept
		{
			return memcmp(reinterpret_cast<const void*>(&data), reinterpret_cast<const void*>(&b.data), sizeof(data)) < 0;
		}

		[[nodiscard]] bool operator <=(const IpAddr& b) const noexcept
		{
			return memcmp(reinterpret_cast<const void*>(&data), reinterpret_cast<const void*>(&b.data), sizeof(data)) <= 0;
		}

		[[nodiscard]] bool operator >(const IpAddr& b) const noexcept
		{
			return memcmp(reinterpret_cast<const void*>(&data), reinterpret_cast<const void*>(&b.data), sizeof(data)) > 0;
		}

		[[nodiscard]] bool operator >=(const IpAddr& b) const noexcept
		{
			return memcmp(reinterpret_cast<const void*>(&data), reinterpret_cast<const void*>(&b.data), sizeof(data)) >= 0;
		}

		void reset() noexcept
		{
			memset(&data, 0, sizeof(data));
		}

		[[nodiscard]] bool isV4() const noexcept
		{
			return IN6_IS_ADDR_V4MAPPED(&data);
		}

		[[nodiscard]] uint32_t getV4() const noexcept // network byte order
		{
			return *reinterpret_cast<const uint32_t*>(reinterpret_cast<uintptr_t>(&data) + 12);
		}

		[[nodiscard]] uint32_t getV4NativeEndian() const noexcept;

	private:
		void setV4()
		{
#if SOUP_WINDOWS
			data.s6_words[0] = 0;
			data.s6_words[1] = 0;
			data.s6_words[2] = 0;
			data.s6_words[3] = 0;
			data.s6_words[4] = 0;
			data.s6_words[5] = 0xffff;
#else
			data.s6_addr16[0] = 0;
			data.s6_addr16[1] = 0;
			data.s6_addr16[2] = 0;
			data.s6_addr16[3] = 0;
			data.s6_addr16[4] = 0;
			data.s6_addr16[5] = 0xffff;
#endif
		}

	public:
		[[nodiscard]] std::string toString() const noexcept
		{
			if (isV4())
			{
				return toString4();
			}
			return toString6();
		}

		[[nodiscard]] std::string toString4() const noexcept
		{
			char buf[INET_ADDRSTRLEN] = { '\0' };
			inet_ntop(AF_INET, reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(&data) + 12), buf, INET_ADDRSTRLEN);
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
