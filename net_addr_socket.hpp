#pragma once

#include "net_addr_ip.hpp"

namespace soup
{
	struct net_addr_socket
	{
		net_addr_ip addr;
		uint16_t port;

		net_addr_socket() noexcept = default;

		explicit net_addr_socket(const std::string& addr, uint16_t port)
			: addr(addr), port(port)
		{
		}

		[[nodiscard]] std::string toString() const noexcept
		{
			std::string str;
			if(addr.isV4())
			{
				str.append(addr.toString4());
			}
			else
			{
				str.push_back('[');
				str.append(addr.toString6());
				str.push_back(']');
			}
			str.push_back(':');
			str.append(std::to_string(port));
			return str;
		}
	};
}
