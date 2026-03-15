#include "ServerWebService.hpp"

#if !SOUP_WASM

#include "HttpRequest.hpp"
#include "MimeType.hpp"
#include "Socket.hpp"
#include "string.hpp"
#include "StringWriter.hpp"
#include "WebSocket.hpp"
#include "WebSocketFrameType.hpp"
#include "WebSocketMessage.hpp"

NAMESPACE_SOUP
{
	struct WebServerClientData
	{
		HttpRequest buffer{};
		bool keep_alive = false;
	};

	struct WebServerWsClientData
	{
		std::string data_buf;
		WebSocketMessage msg_buf;
	};

	ServerWebService::ServerWebService(handle_request_t handle_request)
		: ServerService([](Socket& s, ServerService& srv, Server&) SOUP_EXCAL
		{
			s.disallowRecursion(); // needed for 'excal' guarantee
			static_cast<ServerWebService&>(srv).httpRecv(s);
		}), handle_request(handle_request)
	{
	}

	void ServerWebService::setKeepAlive(Socket& s, bool b)
	{
		s.custom_data.getStructFromMap(WebServerClientData).keep_alive = b;
	}

	void ServerWebService::sendContent(Socket& s, std::string body)
	{
		sendContent(s, "200 OK", std::move(body));
	}

	void ServerWebService::sendContent(Socket& s, const char* status, std::string body)
	{
		HttpResponse resp;
		resp.body = std::move(body);
		sendContent(s, status, std::move(resp));
	}

	void ServerWebService::sendContent(Socket& s, HttpResponse&& resp)
	{
		std::string status = std::to_string(resp.status_code);
		status.push_back(' ');
		status.append(resp.status_text);
		sendContent(s, status.c_str(), std::move(resp));
	}

	void ServerWebService::sendContent(Socket& s, const char* status, HttpResponse&& resp)
	{
		resp.setContentLength();
		resp.setContentType();
		sendResponse(s, status, resp.toString());
	}

	void ServerWebService::sendHtml(Socket& s, const std::string& body)
	{
		sendData(s, MimeType::TEXT_HTML, body, true);
	}

	void ServerWebService::sendHtml(Socket& s, const char* data, size_t size)
	{
		sendData(s, MimeType::TEXT_HTML, data, size, true);
	}

	void ServerWebService::sendText(Socket& s, const std::string& body)
	{
		sendData(s, MimeType::TEXT_PLAIN, body, false);
	}

	void ServerWebService::sendText(Socket& s, const char* data, size_t size)
	{
		sendData(s, MimeType::TEXT_PLAIN, data, size, false);
	}

	void ServerWebService::sendData(Socket& s, const char* mime_type, const char* _data, size_t size, bool is_private)
	{
		std::string data;
		data.reserve(size + 120);
		if (is_private)
		{
			data.append("Cache-Control: private");
		}
		else
		{
			data.append("Access-Control-Allow-Origin: *");
		}
		data.append("\r\nContent-Type: ").append(mime_type);
		data.append("\r\nContent-Length: ").append(std::to_string(size));
		data.append("\r\n\r\n");
		data.append(_data, size);
		sendResponse(s, "200 OK", data);
	}

	void ServerWebService::sendRedirect(Socket& s, const std::string& location)
	{
		std::string cont = "Location: ";
		cont.append(location);
		cont.append("\r\nContent-Length: 0\r\n\r\n");
		sendResponse(s, "302 Found", cont);
	}

	void ServerWebService::send204(Socket& s)
	{
		sendResponse(s, "204 No Content", "Content-Length: 0\r\n\r\n");
	}

	void ServerWebService::send400(Socket& s)
	{
		sendResponse(s, "400 Bad Request", "Content-Length: 0\r\n\r\n");
	}

	void ServerWebService::send404(Socket& s)
	{
		sendResponse(s, "404 Not Found", "Content-Length: 0\r\n\r\n");
	}

	void ServerWebService::send500(Socket& s)
	{
		sendResponse(s, "500 Internal Server Error", "Content-Length: 0\r\n\r\n");
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
	
	void ServerWebService::wsSendText(Socket& s, const std::string& data)
	{
		wsSend(s, WebSocketFrameType::TEXT, data);
	}

	void ServerWebService::wsSendBin(Socket& s, const std::string& data)
	{
		wsSend(s, WebSocketFrameType::BINARY, data);
	}

	void ServerWebService::wsSend(Socket& s, const std::string& data, bool is_text)
	{
		wsSend(s, (is_text ? WebSocketFrameType::TEXT : WebSocketFrameType::BINARY), data);
	}

	void ServerWebService::wsSend(Socket& s, uint8_t opcode, const std::string& payload)
	{
		StringWriter w;
		opcode |= 0x80; // fin
		if (w.u8(opcode))
		{
			if (payload.size() <= 125)
			{
				uint8_t buf = static_cast<uint8_t>(payload.size());
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
				if (uint16_t buf = static_cast<uint16_t>(payload.size()); !w.u16_be(buf))
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
				if (uint64_t buf = payload.size(); !w.u64_be(buf))
				{
					return;
				}
			}
		}
		w.data.append(payload);
		s.send(w.data);
	}

	void ServerWebService::httpRecv(Socket& s)
	{
		s.recv([](Socket& s, std::string&& data, Capture&& cap)
		{
			auto& cd = s.custom_data.getStructFromMap(WebServerClientData);
			auto& req = cd.buffer;

			if (req.method.empty())
			{
				auto method_end = data.find(' ');
				if (method_end == std::string::npos)
				{
				_bad_request:
					s.send("HTTP/1.0 400 Bad Request\r\n\r\n");
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
			}
			else
			{
				req.body.append(data);
			}

			ServerWebService& srv = *cap.get<ServerWebService*>();

			const auto content_length = string::toInt<size_t>(req.findHeader("Content-Length").value_or(std::string{}), 0);
			SOUP_IF_UNLIKELY (req.body.size() < content_length)
			{
				return srv.httpRecv(s);
			}

			if (auto upgrade_value = req.findHeader("Upgrade"))
			{
				if (*upgrade_value == "websocket")
				{
					if (auto key_value = req.findHeader("Sec-WebSocket-Key"))
					{
						if (srv.should_accept_websocket_connection != nullptr
							? srv.should_accept_websocket_connection(s, req, srv)
							: srv.on_websocket_message != nullptr
							)
						{
							// Firefox throws a SkillIssueException if we say HTTP/1.0
							std::string cont = "HTTP/1.1 101 Switching Protocols\r\nConnection: Upgrade\r\nUpgrade: websocket\r\nServer: Soup\r\nSec-WebSocket-Accept: ";
							cont.append(WebSocket::hashKey(*key_value));
							cont.append("\r\n\r\n");
							s.send(cont);

							s.custom_data.removeStructFromMap(WebServerClientData);
							s.custom_data.addStructToMap(WebServerWsClientData, WebServerWsClientData{});

							if (srv.on_websocket_connection_established)
							{
								srv.on_websocket_connection_established(s, req, srv);
							}

							srv.wsRecv(s);
						}
					}
				}
				return;
			}

			if (srv.handle_request)
			{
				if (auto connection_entry = req.findHeader("Connection"))
				{
					if (*connection_entry == "keep-alive")
					{
						cd.keep_alive = true;
					}
				}

				srv.handle_request(s, std::move(req), srv);
				req.clear();

				if (cd.keep_alive)
				{
					srv.httpRecv(s);
				}
			}
		}, this);
	}

	void ServerWebService::wsRecv(Socket& s)
	{
		s.recv([](Socket& s, std::string&& data, Capture&& cap) // on_websocket_message may throw
		{
			auto& cd = s.custom_data.getStructFromMapConst(WebServerWsClientData);
			ServerWebService& srv = *cap.get<ServerWebService*>();

			cd.data_buf.append(data);

			bool fin;
			uint8_t opcode;
			std::string payload;
			WebSocket::ReadFrameStatus status;
			while (status = WebSocket::readFrame(cd.data_buf, fin, opcode, payload), status == WebSocket::OK)
			{
				if (opcode <= WebSocketFrameType::_NON_CONTROL_MAX) // non-control frame
				{
					if (opcode != 0)
					{
						cd.msg_buf.data = std::move(payload);
						cd.msg_buf.is_text = (opcode == WebSocketFrameType::TEXT);
					}
					else
					{
						cd.msg_buf.data.append(payload);
					}

					if (fin)
					{
						if (srv.on_websocket_message)
						{
							srv.on_websocket_message(cd.msg_buf, s, srv);
						}
						cd.msg_buf.data.clear();
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
			if (status == WebSocket::PAYLOAD_INCOMPLETE)
			{
				srv.wsRecv(s);
			}
		}, this);
	}
}

#endif
