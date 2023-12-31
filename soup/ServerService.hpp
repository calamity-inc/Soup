#pragma once

#include "fwd.hpp"

namespace soup
{
	struct ServerService
	{
		using callback_t = void(*)(Socket& client, ServerService&, Server&) SOUP_EXCAL;

		// on_connection_established is called when the TCP connection is established
		callback_t on_connection_established = nullptr;

		// on_tunnel_established is called when:
		// - (non-crypto) the TCP connection is established
		// - (crypto) the TLS handshake has completed
		const callback_t srv_on_tunnel_established;
		callback_t on_tunnel_established = nullptr;

		ServerService(callback_t srv_on_tunnel_established = nullptr)
			: srv_on_tunnel_established(srv_on_tunnel_established)
		{
		}
	};
}
