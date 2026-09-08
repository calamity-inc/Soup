#pragma once

#include "netConnectTask.hpp"
#if SOUP_WINDOWS || SOUP_LINUX
#include "Server.hpp"
#include "ServerWebService.hpp"
#include "WebSocketConnection.hpp"

NAMESPACE_SOUP
{
	// Host implementation for https://github.com/calamity-inc/tru
	// Currently does not support "secure context" clients.
	class truHostTask : public Task
	{
	public:
		netConnectTask relay_connector;
		const std::string id;
	private:
		Server lan_server;
		ServerWebService lan_service;
	public:
		SharedPtr<WebSocketConnection> ctrl_sock;
		SharedPtr<WebSocketConnection> out_sock; // Output. May be a default-initialised if connection failed.
	private:
		bool sent_h = false;
	public:
		bool relaying = false;

	public:
		truHostTask(const IpAddr& relay/*= SOUP_IPV4_NWE(198, 251, 89, 45)*/, std::string id);

		void onTick() final;

	private:
		void ctrlRecv();
	};
}
#endif
