#pragma once

#include "Scheduler.hpp"
#if !SOUP_WASM

#include "type.hpp"

namespace soup
{
	class Server : public Scheduler
	{
	public:
		bool bind(uint16_t port, ServerService* service) noexcept;
		bool bindCrypto(uint16_t port, ServerService* service, tls_server_cert_selector_t cert_selector, tls_server_on_client_hello_t on_client_hello = nullptr) noexcept;
	protected:
		static void setDataAvailableHandler6(Socket& s);
		static void setDataAvailableHandlerCrypto6(Socket& s);
#if SOUP_WINDOWS
		static void setDataAvailableHandler4(Socket& s);
		static void setDataAvailableHandlerCrypto4(Socket& s);
#endif
	};
}
#endif
