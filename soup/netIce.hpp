#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "IpAddr.hpp"

NAMESPACE_SOUP
{
	struct netIce
	{
		// Username is "ufrag" in candidate-attribute and "a=ice-ufrag:" in SDP.
		[[nodiscard]] static bool validateCandidateAsClient(const IpAddr& addr, uint16_t port, const std::string& offer_username, const std::string& answer_username, const std::string& offer_password, unsigned int timeout_ms = 200);
	};
}

#endif
