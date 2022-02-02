#pragma once

#include "addr_ip.hpp"

namespace soup
{
	struct addr_socket
	{
		addr_ip addr;
		uint16_t port;

		addr_socket() noexcept = default;

		explicit addr_socket(const addr_ip& addr, uint16_t port)
			: addr(addr), port(port)
		{
		}
		
		explicit addr_socket(const std::string& addr, uint16_t port)
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
