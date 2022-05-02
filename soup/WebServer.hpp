#pragma once

#include "type.hpp"

#include "Server.hpp"

#include <unordered_set>

namespace soup
{
	class WebServer : public Server
	{
	public:
		using handle_request_t = void(*)(Socket&, HttpRequest&&, WebServer&);
		using log_func_t = void(*)(std::string&&, WebServer&);
		using should_accept_websocket_connection_t = bool(*)(Socket&, const HttpRequest&, WebServer&);
		using on_websocket_message_t = void(*)(WebSocketMessage&, Socket&, WebServer&);

		handle_request_t handle_request;
		log_func_t log_func = nullptr;
		tls_server_on_client_hello_t on_client_hello = nullptr;
		should_accept_websocket_connection_t should_accept_websocket_connection = nullptr;
		on_websocket_message_t on_websocket_message = nullptr;

		WebServer(handle_request_t handle_request);

		static void httpOnAccept(Socket& s, uint16_t port, Server& _srv);

		bool bind(uint16_t port);
		bool bindCrypto(uint16_t port, tls_server_cert_selector_t cert_selector);

		void run();

		// HTTP
		static void sendHtml(Socket& s, std::string body);
		static void sendText(Socket& s, std::string body);
		static void sendData(Socket& s, const char* mime_type, std::string body);
		static void sendRedirect(Socket& s, const std::string& location);
		static void send404(Socket& s);
		static void sendResponse(Socket& s, const char* status, const std::string& headers_and_body);

		// WebSocket
		static void wsSend(Socket& s, const std::string& data, bool is_text);
		static void wsSend(Socket& s, uint8_t opcode, const std::string& payload);

	protected:
		void httpRecv(Socket& s);
		void wsRecv(Socket& s);

		[[nodiscard]] static std::string hashWebSocketKey(std::string key);
	};
}
