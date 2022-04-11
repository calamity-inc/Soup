#pragma once

#include "addr_ip.hpp"

namespace soup
{
	struct addr_socket
	{
		addr_ip ip;
		uint16_t port;

		addr_socket() noexcept = default;

		explicit addr_socket(const addr_ip& ip, uint16_t port)
			: ip(ip), port(port)
		{
		}

		explicit addr_socket(const std::string& ip, uint16_t port)
			: ip(ip), port(port)
		{
		}

		[[nodiscard]] std::string toString() const noexcept
		{
			std::string str;
			if (ip.isV4())
			{
				str.append(ip.toString4());
			}
			else
			{
				str.push_back('[');
				str.append(ip.toString6());
				str.push_back(']');
			}
			str.push_back(':');
			str.append(std::to_string(port));
			return str;
		}
	};
}
