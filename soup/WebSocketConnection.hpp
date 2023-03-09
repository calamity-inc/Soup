#pragma once

#include "Socket.hpp"
#if !SOUP_WASM

namespace soup
{
	class WebSocketConnection : public Socket
	{
	public:
		using callback_t = void(*)(WebSocketConnection&, Capture&&);
		using recv_callback_t = void(*)(WebSocketConnection&, WebSocketMessage&&, Capture&&);

		// Use after connected.
		void upgrade(std::string host, std::string path, callback_t cb, Capture&& cap = {});
		void sendUpgradeRequest(std::string host, std::string path);

		void wsSend(std::string data, bool is_text);
		void wsSend(uint8_t opcode, std::string payload);

		void wsRecv(recv_callback_t cb, Capture&& cap = {});
	};
}

#endif
