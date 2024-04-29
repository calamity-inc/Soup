#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "ServerWebService.hpp"

#include <string>
#include <unordered_map>

NAMESPACE_SOUP
{
	class netMeshService : public ServerWebService
	{
	public:
		using app_msg_handler_t = void(*)(Socket& s, std::string&& data);

		uint64_t link_passnum = 0;
		std::unordered_map<std::string, app_msg_handler_t> app_msg_handlers{};

		netMeshService();

		bool bind(Server& serv);

		static void reply(Socket& s, const std::string& data);
		static void replyAffirmative(Socket& s, const std::string& data = {});
		static void replyNegative(Socket& s, const std::string& data = {});
	protected:
		static void reply(Socket& s, uint8_t msg_type, const std::string& data);
	};
}

#endif
