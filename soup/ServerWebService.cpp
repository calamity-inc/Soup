#include "ServerWebService.hpp"

#if !SOUP_WASM

#include "HttpRequest.hpp"
#include "MimeType.hpp"
#include "Socket.hpp"
#include "StringWriter.hpp"
#include "WebSocket.hpp"
#include "WebSocketFrameType.hpp"
#include "WebSocketMessage.hpp"

NAMESPACE_SOUP
{
	struct WebServerClientData
	{
		bool keep_alive = false;
		std::string recv_buffer;
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
		sendContent(s, "200 OK", std::move(resp));
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
			ServerWebService& srv = *cap.get<ServerWebService*>();
			
			cd.recv_buffer.append(data);

			const auto getContentLength = [](const std::string& headers) -> size_t
			{
				std::string lower_headers = headers;
				for (auto& c : lower_headers)
				{
					if (c >= 'A' && c <= 'Z') c += 32;
				}
				
				auto pos = lower_headers.find("content-length:");
				if (pos == std::string::npos) return 0;
				
				pos += 15;
				while (pos < lower_headers.size() && lower_headers[pos] == ' ') ++pos;
				
				auto end = lower_headers.find("\r\n", pos);
				if (end == std::string::npos) end = lower_headers.size();
				
				try { return std::stoull(headers.substr(pos, end - pos)); }
				catch (...) { return 0; }
			};

			const auto headers_end = cd.recv_buffer.find("\r\n\r\n");
			if (headers_end == std::string::npos)
			{
				srv.httpRecv(s);
				return;
			}

			const auto method_end = cd.recv_buffer.find(' ');
			if (method_end == std::string::npos)
			{
			_bad_request:
				cd.recv_buffer.clear();
				s.send("HTTP/1.0 400 Bad Request\r\n\r\n");
				s.close();
				return;
			}
			
			const auto path_start = method_end + 1;
			const auto path_end = cd.recv_buffer.find(' ', path_start);
			if (path_end == std::string::npos)
			{
				goto _bad_request;
			}
			
			const auto first_line_end = cd.recv_buffer.find("\r\n", path_end);
			if (first_line_end == std::string::npos)
			{
				goto _bad_request;
			}
			const auto message_start = first_line_end + 2;

			// Check Content-Length to determine expected body size
			const size_t body_start = headers_end + 4;
			const std::string headers_section = cd.recv_buffer.substr(message_start, headers_end - message_start);
			const size_t content_length = getContentLength(headers_section);

			// Check if we have received the full body
			const size_t current_body_size = cd.recv_buffer.size() - body_start;
			if (current_body_size < content_length)
			{
				srv.httpRecv(s);
				return;
			}

			// Full request received - build the HttpRequest
			HttpRequest req{};
			req.method = cd.recv_buffer.substr(0, method_end);
			req.path = cd.recv_buffer.substr(path_start, path_end - path_start);
			req.loadMessage(cd.recv_buffer.substr(message_start));

			// Clear buffer for potential keep-alive request
			cd.recv_buffer.clear();
			cd.recv_buffer.shrink_to_fit();

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
						s.custom_data.getStructFromMap(WebServerClientData).keep_alive = true;
					}
				}

				srv.handle_request(s, std::move(req), srv);

				if (s.custom_data.getStructFromMap(WebServerClientData).keep_alive)
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
