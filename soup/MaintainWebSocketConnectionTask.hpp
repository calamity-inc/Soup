#pragma once

#include "Task.hpp"
#if !SOUP_WASM

#include <ctime> // time_t

#include "SharedPtr.hpp"
#include "WebSocketConnection.hpp"

NAMESPACE_SOUP
{
	class MaintainWebSocketConnectionTask : public Task
	{
	protected:
		using on_frame_t = void(*)(WebSocketConnection&, WebSocketMessage&&) SOUP_EXCAL;

		SharedPtr<WebSocketConnection> sock;
		on_frame_t on_frame;
		time_t next_heartbeat;

	public:
		MaintainWebSocketConnectionTask(SharedPtr<WebSocketConnection> sock, on_frame_t on_frame);

		void onTick() final;

	protected:
		void recvLoop(WebSocketConnection& s);

	public:
		[[nodiscard]] std::string toString() const SOUP_EXCAL final;
	};
}

#endif
