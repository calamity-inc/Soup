#pragma once

#include "type.hpp"

#include "Scheduler.hpp"

namespace soup
{
	class Server : public Scheduler
	{
	public:
		using callback_t = void(*)(Socket& client, uint16_t port, Server&);

		bool bind(uint16_t port, callback_t on_connection_established) noexcept;
		bool bindCrypto(uint16_t port, tls_server_cert_selector_t cert_selector, callback_t on_tunnel_established, tls_server_on_client_hello_t on_client_hello = nullptr, callback_t on_connection_established = nullptr) noexcept;
	protected:
		static void setDataAvailableHandler6(Socket& s);
		static void setDataAvailableHandlerCrypto6(Socket& s);
#if SOUP_WINDOWS
		static void setDataAvailableHandler4(Socket& s);
		static void setDataAvailableHandlerCrypto4(Socket& s);
#endif
	};
}
