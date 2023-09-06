#pragma once

#if !SOUP_WASM

#include <string>

#include "Endian.hpp"
#include "MacAddr.hpp"

namespace soup
{
	struct dhcp
	{
		struct Info
		{
			std::string server_name{};
			network_u32_t dns_server = 0;

			[[nodiscard]] bool isValid() const noexcept
			{
				return !server_name.empty()
					|| dns_server != 0
					;
			}
		};

		[[nodiscard]] static Info requestInfo(native_u32_t adaptor_ip_addr = -1);

		static native_u32_t requestAddress(native_u32_t server, native_u32_t netmask, const MacAddr& mac_addr = MacAddr::SOUP_FIRST);
		static bool requestSpecificAddress(native_u32_t server, native_u32_t addr, const MacAddr& mac_addr = MacAddr::SOUP_FIRST);
	};
}

#endif
