#include "WebSocketConnection.hpp"

#if SOUP_WASM || SOUP_CODE_INSPECTOR

namespace soup
{
	WebSocketConnection::WebSocketConnection(const std::string& url)
	{
		EmscriptenWebSocketCreateAttributes ws_attrs;
		emscripten_websocket_init_create_attributes(&ws_attrs);
		ws_attrs.url = url.c_str();
		ws_attrs.createOnMainThread = EM_TRUE;
		ws = emscripten_websocket_new(&ws_attrs);
	}

	static EM_BOOL em_onopen(int eventType, const EmscriptenWebSocketOpenEvent* e, void* userData)
	{
		WebSocketConnection con{ e->socket };
		reinterpret_cast<WebSocketConnection::on_open_t>(userData)(con);
		return EM_TRUE;
	}

	void WebSocketConnection::onOpen(on_open_t f)
	{
		emscripten_websocket_set_onopen_callback(ws, reinterpret_cast<void*>(f), &em_onopen);
	}

	static EM_BOOL em_onmessage(int eventType, const EmscriptenWebSocketMessageEvent* e, void* userData)
	{
		WebSocketConnection con{ e->socket };
		WebSocketMessage msg{
			std::string((const char*)e->data, (size_t)e->numBytes),
			(bool)e->isText
		};
		reinterpret_cast<WebSocketConnection::on_message_t>(userData)(con, msg);
		return EM_TRUE;
	}

	void WebSocketConnection::onMessage(on_message_t f)
	{
		emscripten_websocket_set_onmessage_callback(ws, reinterpret_cast<void*>(f), &em_onmessage);
	}

	static EM_BOOL em_onclose(int eventType, const EmscriptenWebSocketCloseEvent* e, void* userData)
	{
		WebSocketConnection con{ e->socket };
		reinterpret_cast<WebSocketConnection::on_close_t>(userData)(con);
		return EM_TRUE;
	}

	void WebSocketConnection::onClose(on_close_t f)
	{
		emscripten_websocket_set_onclose_callback(ws, reinterpret_cast<void*>(f), &em_onclose);
	}
}

#endif
