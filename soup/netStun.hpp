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

		// Asks the server to 'hit us' on UDP/target_port. If you have that port bound and see traffic there, you know it's reachable!
		// This requires a server supporting RFC5780 (an extension to STUN): https://github.com/muink/rfc5780-stun-server/blob/master/valid_hosts_rfc5780.txt
		static void requestTraffic(const IpAddr& server_addr, uint16_t server_port, uint16_t target_port);
		static bool isExternallyReachable(const IpAddr& server_addr, uint16_t server_port, uint16_t target_port, unsigned int timeout_ms = 1000);

		static void addMessageIntegrity(std::string& data, const std::string& key) SOUP_EXCAL;
		static void addFingerprint(std::string& data) noexcept;
	};
}
#endif
