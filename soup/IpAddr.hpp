#pragma once

#include <cstdint>
#include <cstring> // memcpy
#include <string>

#include "base.hpp"
#include "fwd.hpp"

#if SOUP_WINDOWS
#pragma comment(lib, "Ws2_32.lib")
#include <Ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

#include "Endian.hpp"
#include "string.hpp"
#include <regex>
#include <iostream>

namespace soup
{
	class IpAddr
	{
	public:
		in6_addr data;

		IpAddr() noexcept = default;

		IpAddr(const char* str)
		{
			if (strcmp(str, "localhost") == 0) str = "127.0.0.1";

			if (strstr(str, ".") == nullptr)
			{
				inet_pton(AF_INET6, str, &data);
			}
			else
			{
				maskToV4();
				inet_pton(AF_INET, str, reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(&data) + 12));
			}
		}

		IpAddr(const std::string& str)
		{
			std::string value = str;
			
			if (str == "localhost") value = "127.0.0.1";
			
			if (value.find('.') == std::string::npos)
			{
				inet_pton(AF_INET6, value.data(), &data);
			}
			else
			{
				maskToV4();
				inet_pton(AF_INET, value.data(), reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(&data) + 12));
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

		explicit IpAddr(const network_u32_t ipv4) noexcept
		{
			operator =(ipv4);
		}

		explicit IpAddr(const native_u32_t ipv4) noexcept
		{
			operator =(ipv4);
		}

		bool fromString(const std::string& str);

		void operator = (const network_u32_t ipv4) noexcept
		{
			maskToV4();
			*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(&data) + 12) = ipv4;
		}

		void operator = (const native_u32_t ipv4) noexcept
		{
			operator = (Endianness::toNetwork(ipv4));
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

		[[nodiscard]] network_u32_t getV4() const noexcept
		{
			return *reinterpret_cast<const network_u32_t*>(reinterpret_cast<uintptr_t>(&data) + 12);
		}

		[[nodiscard]] native_u32_t getV4NativeEndian() const noexcept;

	private:
		void maskToV4()
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

		[[nodiscard]] bool isPrivateIp() const noexcept
		{
			if (isV4())
			{
				auto v4 = toString4();
				std::regex privateRangeRegexV4("^(127\\.)|(192\\.168\\.)|(10\\.)|(172\\.1[6-9]\\.)|(172\\.2[0-9]\\.)|(172\\.3[0-1]\\.)|(::1$)");
				return regex_search(v4, privateRangeRegexV4);
			}
			return false;
		}

		[[nodiscard]] std::string getArpaName() const;
		[[nodiscard]] std::string getReverseDns() const;
		[[nodiscard]] std::string getReverseDns(dnsResolver& resolver) const;
	};
}
