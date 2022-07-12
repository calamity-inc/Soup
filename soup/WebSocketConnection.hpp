#pragma once

#include "base.hpp"

#if SOUP_WASM || SOUP_CODE_INSPECTOR
#include "WebSocketMessage.hpp"

#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>

namespace soup
{
	// Emscripten documentation kinda poo, but there's this:
	// - https://github.com/emscripten-core/emscripten/blob/main/system/include/emscripten/websocket.h
	// - https://gist.github.com/nus/564e9e57e4c107faa1a45b8332c265b9
	// Need to link with -lwebsocket.js

	class WebSocketConnection
	{
	public:
		using on_open_t = void(*)();
		using on_message_t = void(*)(const WebSocketMessage&);
		using on_close_t = void(*)();

	private:
		EMSCRIPTEN_WEBSOCKET_T ws;

	public:
		explicit WebSocketConnection(const std::string& url); // url e.g. wss://echo.websocket.org

		void onOpen(on_open_t f);
		void onMessage(on_message_t f);
		void onClose(on_close_t f);
	};
}

#endif
