#pragma once

#include "type.hpp"

#include "Server.hpp"

#include <unordered_set>

namespace soup
{
	class WebServer : public Server
	{
	public:
		using handle_request_t = void(*)(Socket& client, HttpRequest&& req);
		using log_func_t = void(*)(std::string&&, WebServer&);

		handle_request_t handle_request;
		tls_server_cert_selector_t cert_selector;
		log_func_t log_func = nullptr;
		tls_server_on_client_hello_t on_client_hello = nullptr;

	protected:
		using Server::on_accept;

		std::unordered_set<uint16_t> secure_ports{};

	public:
		WebServer(handle_request_t handle_request);

		bool bindSecure(uint16_t port);

		void run();

		static void sendHtml(Socket& s, std::string body);
		static void sendText(Socket& s, std::string body);
		static void sendData(Socket& s, const char* mime_type, std::string body);
		static void sendRedirect(Socket& s, const std::string& location);
		static void sendResponse(Socket& s, const char* status, const std::string& headers_and_body);

	protected:
		void httpRecv(Socket& s);
	};
}
