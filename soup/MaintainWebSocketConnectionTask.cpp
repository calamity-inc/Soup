#include "MaintainWebSocketConnectionTask.hpp"
#if !SOUP_WASM

#include "time.hpp"
#include "WebSocketFrameType.hpp"

#define HEARTBEAT_INTERVAL_MS (5 * 60 * 1000)

NAMESPACE_SOUP
{
	MaintainWebSocketConnectionTask::MaintainWebSocketConnectionTask(SharedPtr<WebSocketConnection> sock, on_frame_t on_frame)
		: sock(std::move(sock)), on_frame(on_frame), next_heartbeat(time::millis() + HEARTBEAT_INTERVAL_MS)
	{
		recvLoop(*this->sock);
	}

	void MaintainWebSocketConnectionTask::onTick()
	{
		SOUP_IF_UNLIKELY (sock->isWorkDoneOrClosed())
		{
			setWorkDone();
			return;
		}
		SOUP_IF_UNLIKELY (time::millis() >= next_heartbeat)
		{
			sock->wsSend(WebSocketFrameType::PING, {});
			next_heartbeat = time::millis() + HEARTBEAT_INTERVAL_MS;
		}
	}

	void MaintainWebSocketConnectionTask::recvLoop(WebSocketConnection& s)
	{
		s.wsRecv([](WebSocketConnection& s, WebSocketMessage&& msg, Capture&& cap) SOUP_EXCAL
		{
			auto task = cap.get<MaintainWebSocketConnectionTask*>();
			if (!msg.data.empty())
			{
				task->next_heartbeat = time::millis() + HEARTBEAT_INTERVAL_MS;
				task->on_frame(s, std::move(msg));
			}
			task->recvLoop(s);
		}, this);
	}
}
#endif
