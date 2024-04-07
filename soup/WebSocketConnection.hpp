#pragma once

#include "Socket.hpp"
#if !SOUP_WASM

#include "Promise.hpp"
#include "SharedPtr.hpp"
#include "WebSocketMessage.hpp"

NAMESPACE_SOUP
{
	class WebSocketConnection : public Socket
	{
	public:
		using callback_t = void(*)(WebSocketConnection&, Capture&&) SOUP_EXCAL;
		using recv_callback_t = void(*)(WebSocketConnection&, WebSocketMessage&&, Capture&&) SOUP_EXCAL;

		// Use after connected.
		void upgrade(std::string host, std::string path, callback_t cb, Capture&& cap = {}) SOUP_EXCAL;
		void sendUpgradeRequest(std::string host, std::string path) SOUP_EXCAL;

		void wsSend(std::string data, bool is_text = true) SOUP_EXCAL;
		void wsSend(uint8_t opcode, std::string payload) SOUP_EXCAL;

		void wsRecv(recv_callback_t cb, Capture&& cap = {}) SOUP_EXCAL;
		[[nodiscard]] SharedPtr<Promise<WebSocketMessage>> wsRecv() SOUP_EXCAL;
	};
}

#endif
