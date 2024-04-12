#pragma once

#include "base.hpp"
#if !SOUP_WASM
#include "IpAddr.hpp"

NAMESPACE_SOUP
{
	struct netStun
	{
		// Asks the server what they see our IP address as.
		// There's tons of servers running this protocol on the internet: https://gist.github.com/sagivo/3a4b2f2c7ac6e1b5267c2f1f59ac6c6b
		[[nodiscard]] static IpAddr queryBinding(const IpAddr& server_addr, uint16_t server_port, unsigned int timeout_ms = 200);

		static void addMessageIntegrity(std::string& data, const std::string& key) SOUP_EXCAL;
		static void addFingerprint(std::string& data) noexcept;
	};
}
#endif
