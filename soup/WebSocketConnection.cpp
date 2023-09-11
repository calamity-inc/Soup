#include "WebSocketConnection.hpp"
#if !SOUP_WASM

#include "HttpRequest.hpp"
#include "rand.hpp"
#include "StringWriter.hpp"
#include "WebSocket.hpp"
#include "WebSocketFrameType.hpp"
#include "WebSocketMessage.hpp"

namespace soup
{
	struct CaptureWsUpgrade
	{
		WebSocketConnection::callback_t cb;
		Capture cap;
	};

	static void upgradeRecvLoop(Socket& s, Capture&& cap)
	{
		s.recv([](Socket& s, std::string&& data, Capture&& cap)
		{
			if (data.find("\r\n\r\n") != std::string::npos)
			{
				CaptureWsUpgrade& c = cap.get<CaptureWsUpgrade>();
				c.cb(static_cast<WebSocketConnection&>(s), std::move(c.cap));
			}
			else
			{
				upgradeRecvLoop(s, std::move(cap));
			}
		}, std::move(cap));
	}

	void WebSocketConnection::upgrade(std::string host, std::string path, callback_t cb, Capture&& cap)
	{
		sendUpgradeRequest(std::move(host), std::move(path));
		upgradeRecvLoop(*this, CaptureWsUpgrade{ cb, std::move(cap) });
	}

	void WebSocketConnection::sendUpgradeRequest(std::string host, std::string path)
	{
		HttpRequest req(std::move(host), std::move(path));
		req.header_fields.at("Connection") = "Upgrade";
		req.header_fields.emplace("Upgrade", "websocket");
		req.header_fields.emplace("Sec-WebSocket-Key", WebSocket::generateKey());
		req.header_fields.emplace("Sec-WebSocket-Version", "13");
		req.send(*this);
	}

	void WebSocketConnection::wsSend(std::string data, bool is_text)
	{
		wsSend((is_text ? WebSocketFrameType::TEXT : WebSocketFrameType::BINARY), data);
	}

	void WebSocketConnection::wsSend(uint8_t opcode, std::string payload)
	{
		StringWriter w{ false };
		opcode |= 0x80; // fin
		if (w.u8(opcode))
		{
			if (payload.size() <= 125)
			{
				uint8_t buf = payload.size();
				buf |= 0x80; // has mask
				if (!w.u8(buf))
				{
					return;
				}
			}
			else if (payload.size() <= 0xFFFF)
			{
				if (uint8_t buf = (126 | 0x80); !w.u8(buf))
				{
					return;
				}
				if (uint16_t buf = payload.size(); !w.u16(buf))
				{
					return;
				}
			}
			else
			{
				if (uint8_t buf = (127 | 0x80); !w.u8(buf))
				{
					return;
				}
				if (uint64_t buf = payload.size(); !w.u64(buf))
				{
					return;
				}
			}
		}

		uint8_t mask[4];
		rand.fill(mask);

		for (auto i = 0; i != payload.size(); ++i)
		{
			payload[i] ^= mask[i % 4];
		}

		if (!w.str(4, (const char*)mask))
		{
			return;
		}

		w.data.append(payload);
		this->send(w.data);
	}

	struct WsRecvBuffer
	{
		std::string buf;
	};

	struct CaptureWsRecv
	{
		WebSocketConnection::recv_callback_t cb;
		Capture cap;
	};

	void WebSocketConnection::wsRecv(recv_callback_t cb, Capture&& cap)
	{
		recv([](Socket& s, std::string&& app, Capture&& _cap)
		{
			auto& buf = s.custom_data.getStructFromMap(WsRecvBuffer).buf;
			auto& cap = _cap.get<CaptureWsRecv>();

			buf.append(app);

			bool fin;
			uint8_t opcode;
			std::string payload;
			while (true)
			{
				auto res = WebSocket::readFrame(buf, fin, opcode, payload);
				if (res != WebSocket::OK)
				{
					if (res == WebSocket::PAYLOAD_INCOMPLETE)
					{
						static_cast<WebSocketConnection&>(s).wsRecv(cap.cb, std::move(cap.cap));
					}
					break;
				}

				SOUP_ASSERT(fin);
				SOUP_ASSERT(opcode == WebSocketFrameType::TEXT || opcode == WebSocketFrameType::BINARY);

				WebSocketMessage msg;
				msg.is_text = (opcode == WebSocketFrameType::TEXT);
				msg.data = std::move(payload);

				cap.cb(static_cast<WebSocketConnection&>(s), std::move(msg), std::move(cap.cap));
			}
		}, CaptureWsRecv{ cb, std::move(cap) });
	}

	SharedPtr<Promise<WebSocketMessage>> WebSocketConnection::wsRecv()
	{
		auto p = soup::make_shared<Promise<WebSocketMessage>>();
		wsRecv([](WebSocketConnection&, WebSocketMessage&& msg, Capture&& cap)
		{
			cap.get<SharedPtr<Promise<WebSocketMessage>>>()->fulfil(std::move(msg));
		}, p);
		return p;
	}
}

#endif
