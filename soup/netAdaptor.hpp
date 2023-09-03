#pragma once

#include "base.hpp"

#if SOUP_WINDOWS

#include <string>
#include <vector>

#include "Endian.hpp"
#include "MacAddr.hpp"

namespace soup
{
	struct netAdaptor
	{
		std::string name;
		MacAddr mac_addr;
		native_u32_t ip_addr;
		native_u32_t netmask;

		[[nodiscard]] constexpr bool hasNetwork() const noexcept
		{
			return ip_addr != 0;
		}

		[[nodiscard]] static std::vector<netAdaptor> getAll();
	};
}

#endif
