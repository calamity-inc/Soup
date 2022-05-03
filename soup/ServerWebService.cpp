#include "ServerWebService.hpp"

#include "base64.hpp"
#include "HttpRequest.hpp"
#include "sha1.hpp"
#include "Socket.hpp"
#include "StringReader.hpp"
#include "StringWriter.hpp"
#include "WebSocketFrameType.hpp"
#include "WebSocketMessage.hpp"

namespace soup
{
	struct WebServerClientData
	{
		bool keep_alive = false;
	};

	ServerWebService::ServerWebService(handle_request_t handle_request)
		: ServerService([](Socket& s, ServerService& srv, Server&)
		{
			reinterpret_cast<ServerWebService&>(srv).httpRecv(s);
		}), handle_request(handle_request)
	{
	}

	void ServerWebService::sendHtml(Socket& s, std::string body)
	{
		sendData(s, "text/html", std::move(body));
	}

	void ServerWebService::sendText(Socket& s, std::string body)
	{
		sendData(s, "text/plain", std::move(body));
	}

	void ServerWebService::sendData(Socket& s, const char* mime_type, std::string body)
	{
		auto len = body.size();
		body.insert(0, "\r\n\r\n");
		body.insert(0, std::to_string(len));
		body.insert(0, "\r\nContent-Length: ");
		body.insert(0, mime_type);
		body.insert(0, "Cache-Control: private\r\nContent-Type: ");
		sendResponse(s, "200", body);
	}

	void ServerWebService::sendRedirect(Socket& s, const std::string& location)
	{
		std::string cont = "Location: ";
		cont.append(location);
		cont.append("\r\nContent-Length: 0\r\n\r\n");
		sendResponse(s, "302", cont);
	}

	void ServerWebService::send404(Socket& s)
	{
		sendResponse(s, "404", "Content-Length: 0\r\n\r\n");
	}

	void ServerWebService::sendResponse(Socket& s, const char* status, const std::string& headers_and_body)
	{
		std::string cont = "HTTP/1.0 ";
		cont.append(status);
		cont.append("\r\nServer: Soup\r\nConnection: ");
		cont.append(s.custom_data.getStructFromMap(WebServerClientData).keep_alive ? "keep-alive" : "close");
		cont.append("\r\n");
		cont.append(headers_and_body);
		s.send(std::move(cont));
	}
	
	void ServerWebService::wsSend(Socket& s, const std::string& data, bool is_text)
	{
		wsSend(s, (is_text ? WebSocketFrameType::TEXT : WebSocketFrameType::BINARY), data);
	}

	void ServerWebService::wsSend(Socket& s, uint8_t opcode, const std::string& payload)
	{
		StringWriter w{ false };
		opcode |= 0x80; // fin
		if (w.u8(opcode))
		{
			if (payload.size() <= 125)
			{
				uint8_t buf = payload.size();
				if (!w.u8(buf))
				{
					return;
				}
			}
			else if (payload.size() <= 0xFFFF)
			{
				if (uint8_t buf = 126; !w.u8(buf))
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
				if (uint8_t buf = 127; !w.u8(buf))
				{
					return;
				}
				if (uint64_t buf = payload.size(); !w.u64(buf))
				{
					return;
				}
			}
		}
		w.str.append(payload);
		s.send(w.str);
	}

	void ServerWebService::httpRecv(Socket& s)
	{
		s.recv([](Socket& s, std::string&& data, Capture&& cap)
		{
			HttpRequest req{};
			auto method_end = data.find(' ');
			if (method_end == std::string::npos)
			{
			_bad_request:
				s.send("HTTP/1.0 400\r\n\r\n");
				s.close();
				return;
			}
			req.method = data.substr(0, method_end);
			method_end += 1;
			auto path_end = data.find(' ', method_end);
			if (path_end == std::string::npos)
			{
				goto _bad_request;
			}
			req.path = data.substr(method_end, path_end - method_end);
			path_end += 1;
			auto message_start = data.find("\r\n", path_end);
			if (message_start == std::string::npos)
			{
				goto _bad_request;
			}
			message_start += 2;
			req.loadMessage(data.substr(message_start));

			ServerWebService& srv = *cap.get<ServerWebService*>();

			if (auto upgrade_entry = req.header_fields.find("Upgrade"); upgrade_entry != req.header_fields.end())
			{
				if (upgrade_entry->second == "websocket")
				{
					if (auto key_entry = req.header_fields.find("Sec-WebSocket-Key"); key_entry != req.header_fields.end())
					{
						if (srv.should_accept_websocket_connection
							&& srv.should_accept_websocket_connection(s, req, srv)
							)
						{
							std::string cont = "HTTP/1.0 101\r\nConnection: Upgrade\r\nUpgrade: websocket\r\nServer: Soup\r\nSec-WebSocket-Accept: ";
							cont.append(hashWebSocketKey(key_entry->second));
							cont.append("\r\n\r\n");
							s.send(cont);

							s.custom_data.removeStructFromMap(WebServerClientData);

							srv.wsRecv(s);
						}
					}
				}
				return;
			}

			if (auto connection_entry = req.header_fields.find("Connection"); connection_entry != req.header_fields.end())
			{
				if (connection_entry->second == "keep-alive")
				{
					s.custom_data.getStructFromMap(WebServerClientData).keep_alive = true;
				}
			}

			srv.handle_request(s, std::move(req), srv);

			if (s.custom_data.getStructFromMap(WebServerClientData).keep_alive)
			{
				srv.httpRecv(s);
			}
		}, this);
	}

	void ServerWebService::wsRecv(Socket& s)
	{
		s.recv([](Socket& s, std::string&& data, Capture&& cap)
		{
			StringReader r{ std::move(data), false };

			if (uint8_t buf; r.u8(buf))
			{
				bool fin = (buf >> 7);
				uint8_t opcode = (buf & 0x7F);

				if (uint8_t buf; r.u8(buf))
				{
					bool has_mask = (buf >> 7);
					uint64_t payload_len = (buf & 0x7F);

					if (payload_len == 126)
					{
						uint16_t buf;
						if (!r.u16(buf))
						{
							return;
						}
						payload_len = buf;
					}
					else if (payload_len == 127)
					{
						if (!r.u64(payload_len))
						{
							return;
						}
					}

					std::string mask;
					if (has_mask)
					{
						if (!r.str(4, mask))
						{
							return;
						}
					}

					std::string payload;
					if (!r.str(payload_len, payload))
					{
						return;
					}

					if (has_mask)
					{
						for (auto i = 0; i != payload.size(); ++i)
						{
							payload[i] ^= mask.at(i % 4);
						}
					}

					ServerWebService& srv = *cap.get<ServerWebService*>();

					if (opcode <= WebSocketFrameType::_NON_CONTROL_MAX) // non-control frame
					{
						WebSocketMessage& msg_buf = s.custom_data.getStructFromMap(WebSocketMessage);

						if (opcode != 0)
						{
							msg_buf.data = std::move(payload);
							msg_buf.is_text = (opcode == 1);
						}
						else
						{
							msg_buf.data.append(payload);
						}

						if (fin)
						{
							if (srv.on_websocket_message)
							{
								srv.on_websocket_message(msg_buf, s, srv);
							}
							msg_buf.data.clear();
						}
					}
					else // control frame
					{
						if (opcode == WebSocketFrameType::PING)
						{
							wsSend(s, WebSocketFrameType::PONG, payload);
						}
						else if (opcode != WebSocketFrameType::PONG)
						{
							s.close();
							return;
						}
					}

					srv.wsRecv(s);
				}
			}
		}, this);
	}

	std::string ServerWebService::hashWebSocketKey(std::string key)
	{
		key.append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
		return base64::encode(sha1::hash(key));
	}
}
