#pragma once

#include "base.hpp"

#if SOUP_WASM || SOUP_CODE_INSPECTOR
#include "WebSocketMessage.hpp"

#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>

NAMESPACE_SOUP
{
	// Emscripten documentation kinda poo, but there's this:
	// - https://github.com/emscripten-core/emscripten/blob/main/system/include/emscripten/websocket.h
	// - https://gist.github.com/nus/564e9e57e4c107faa1a45b8332c265b9
	// Need to link with -lwebsocket.js

	class WebSocketConnectionEmscripten
	{
	public:
		using on_open_t = void(*)(WebSocketConnectionEmscripten&);
		using on_message_t = void(*)(WebSocketConnectionEmscripten&, const WebSocketMessage&);
		using on_close_t = void(*)(WebSocketConnectionEmscripten&);

	private:
		EMSCRIPTEN_WEBSOCKET_T ws;

	public:
		explicit WebSocketConnectionEmscripten(const std::string& url); // url e.g. wss://echo.websocket.org

		explicit WebSocketConnectionEmscripten(EMSCRIPTEN_WEBSOCKET_T ws)
			: ws(ws)
		{
		}

		void onOpen(on_open_t f);
		void onMessage(on_message_t f);
		void onClose(on_close_t f);
	};
}
#endif
