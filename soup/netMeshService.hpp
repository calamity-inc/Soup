#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "ServerService.hpp"

#include <string>

#include "netMeshMsgType.hpp"

namespace soup
{
	class netMeshService : public ServerService
	{
	public:
		using app_msg_handler_t = bool(*)(netMeshMsgType, std::string&& data);

		uint64_t link_passnum = 0;
		app_msg_handler_t app_msg_handler = nullptr;

		netMeshService()
			: ServerService(&onTunnelEstablished)
		{
		}

		bool bind(Server& serv);

	private:
		static void onTunnelEstablished(Socket& s, ServerService&, Server&) SOUP_EXCAL;
	};
	inline netMeshService g_mesh_service;
}

#endif
