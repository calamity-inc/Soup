#include "HttpRequest.hpp"

#if !SOUP_WASM

#include "Callback.hpp"
#include "joaat.hpp"
#include "ObfusString.hpp"
#include "Scheduler.hpp"
#include "Socket.hpp"
#include "UniquePtr.hpp"
#include "Uri.hpp"
#include "urlenc.hpp"

namespace soup
{
	HttpRequest::HttpRequest(std::string method, std::string host, std::string path)
		: MimeMessage({
			{ObfusString("Host"), std::move(host)},
			{ObfusString("User-Agent"), ObfusString("Mozilla/5.0 (compatible; Soup Library; +https://soup.do)")},
			{ObfusString("Connection"), ObfusString("close")},
			{ObfusString("Accept-Encoding"), ObfusString("deflate, gzip")},
		}), method(std::move(method)), path(std::move(path))
	{
		fixPath();
	}

	HttpRequest::HttpRequest(std::string host, std::string path)
		: HttpRequest(ObfusString("GET"), std::move(host), std::move(path))
	{
	}

	HttpRequest::HttpRequest(const Uri& uri)
		: HttpRequest(uri.host, uri.getRequestPath())
	{
		if (joaat::hash(uri.scheme) == joaat::hash("http"))
		{
			use_tls = false;
			port = 80;
		}

		if (uri.port != 0)
		{
			port = uri.port;
		}
	}

	const std::string& HttpRequest::getHost() const
	{
		return header_fields.at(ObfusString("Host"));
	}

	void HttpRequest::setPath(std::string&& path)
	{
		this->path = std::move(path);
		fixPath();
	}

	void HttpRequest::fixPath()
	{
		if (path.c_str()[0] != '/')
		{
			path.insert(0, 1, '/');
		}
	}

	void HttpRequest::setPayload(std::string payload)
	{
		header_fields.emplace(ObfusString("Content-Length"), std::to_string(payload.size()));
		body = std::move(payload);
	}

	struct CaptureHttpRequestExecute
	{
		const HttpRequest* req;
		std::string* resp;
	};

	std::optional<HttpResponse> HttpRequest::execute() const
	{
		return execute(&Socket::certchain_validator_relaxed);
	}

	std::optional<HttpResponse> HttpRequest::execute(bool(*certchain_validator)(const X509Certchain&, const std::string& server_name)) const
	{
		auto sock = make_unique<Socket>();
		std::string resp{};
		const auto& host = getHost();
		if (sock->connect(host, port))
		{
			Scheduler sched{};
			Socket& s = sched.addSocket(std::move(sock));
			if (use_tls)
			{
				s.enableCryptoClient(host, [](Socket& s, Capture&& _cap)
				{
					auto& cap = _cap.get<CaptureHttpRequestExecute>();
					cap.req->send(s);
					execute_tick(s, cap.resp);
				}, CaptureHttpRequestExecute{ this, &resp }, certchain_validator);
			}
			else
			{
				send(s);
				execute_tick(s, &resp);
			}
			sched.run();
		}
		if (!resp.empty())
		{
			auto i = resp.find("\r\n");
			if (i != std::string::npos)
			{
				auto arr = string::explode(resp.substr(0, i), ' ');
				if (arr.size() >= 2)
				{
					resp.erase(0, i + 2);

					HttpResponse res = std::move(resp);
					res.status_code = string::toInt<uint16_t>(arr.at(1), 0);

					if (auto enc = res.header_fields.find(ObfusString("Transfer-Encoding")); enc != res.header_fields.end())
					{
						if (joaat::hash(enc->second) == joaat::hash("chunked"))
						{
							size_t i = 0;
							while (true)
							{
								const auto chunk_size_end = res.body.find("\r\n", i);
								if (chunk_size_end == std::string::npos)
								{
									break;
								}
								const auto chunk_size_len = (chunk_size_end - i);
								unsigned long long chunk_size = 0;
								try
								{
									chunk_size = std::stoull(res.body.substr(i, chunk_size_len), nullptr, 16);
								}
								catch (...)
								{
								}
								res.body.erase(i, chunk_size_len + 2); // erase chunk size + CRLF
								i += chunk_size;
								res.body.erase(i, 2); // erase CRLF after chunk-data
								if (chunk_size == 0)
								{
									break;
								}
							}
						}
					}

					res.decode();

					if (!isChallengeResponse(res))
					{
						return std::optional<HttpResponse>(std::move(res));
					}
				}
			}
		}
		return {};
	}

	void HttpRequest::send(Socket& s) const
	{
		std::string data{};
		data.append(method);
		data.push_back(' ');
		data.append(path_is_encoded ? path : urlenc::encodePathWithQuery(path));
		data.append(ObfusString(" HTTP/1.1").str());
		data.append("\r\n");
		data.append(toString());
		s.send(data);
	}

	void HttpRequest::execute_tick(Socket& s, std::string* resp)
	{
		s.recv([](Socket& s, std::string&& app, Capture&& cap)
		{
			std::string* resp = cap.get<std::string*>();
			resp->append(std::move(app));
			execute_tick(s, resp);
		}, resp);
	}

	bool HttpRequest::isChallengeResponse(const HttpResponse& res)
	{
		return res.body.find(ObfusString(R"(href="https://www.cloudflare.com?utm_source=challenge)").str()) != std::string::npos;
	}

	struct HttpResponseReceiver
	{
		enum Status : uint8_t
		{
			CODE,
			HEADER,
			BODY_CHUNKED,
			BODY_LEN,
			BODY_CLOSE,
		};

		std::string buf{};
		HttpResponse resp{};
		Status status = CODE;
		unsigned long long bytes_remain = 0;

		Callback<void(Socket&, std::optional<HttpResponse>&&)> callback;

		HttpResponseReceiver(void fp(Socket&, std::optional<HttpResponse>&&, Capture&&), Capture&& cap)
			: callback(fp, std::move(cap))
		{
		}

		void tick(Socket& s, Capture&& cap)
		{
			s.recv([](Socket& s, std::string&& app, Capture&& cap)
			{
				auto& self = cap.get<HttpResponseReceiver>();
				if (app.empty())
				{
					// Connection was closed and no more data
					if (self.status == BODY_CLOSE)
					{
						self.resp.body = std::move(self.buf);
						self.callbackSuccess(s);
					}
					else
					{
						self.callback(s, std::nullopt);
					}
					return;
				}
				self.buf.append(app);
				while (true)
				{
					if (self.status == CODE)
					{
						auto i = self.buf.find("\r\n");
						if (i == std::string::npos)
						{
							break;
						}
						auto arr = string::explode(self.buf.substr(0, i), ' ');
						if (arr.size() < 2)
						{
							// Invalid data
							self.callback(s, std::nullopt);
							return;
						}
						self.buf.erase(0, i + 2);
						self.resp.status_code = string::toInt<uint16_t>(arr.at(1), 0);
						self.status = HEADER;
					}
					else if (self.status == HEADER)
					{
						auto i = self.buf.find("\r\n");
						if (i == std::string::npos)
						{
							break;
						}
						auto line = self.buf.substr(0, i);
						self.buf.erase(0, i + 2);
						SOUP_IF_LIKELY (!line.empty())
						{
							self.resp.addHeader(line);
						}
						else
						{
							if (auto enc = self.resp.header_fields.find(ObfusString("Transfer-Encoding")); enc != self.resp.header_fields.end())
							{
								if (joaat::hash(enc->second) == joaat::hash("chunked"))
								{
									self.status = BODY_CHUNKED;
								}
							}
							if (self.status == HEADER)
							{
								if (auto len = self.resp.header_fields.find(ObfusString("Content-Length")); len != self.resp.header_fields.end())
								{
									self.status = BODY_LEN;
									try
									{
										self.bytes_remain = std::stoull(len->second);
									}
									catch (...)
									{
										self.callback(s, std::nullopt);
										return;
									}
								}
								else
								{
									if (auto con = self.resp.header_fields.find(ObfusString("Connection")); con != self.resp.header_fields.end())
									{
										if (joaat::hash(con->second) == joaat::hash("close"))
										{
											self.status = BODY_CLOSE;
											s.callback_recv_on_close = true;
										}
									}
									if (self.status == HEADER)
									{
										// We still have no idea how to read the body. Assuming response is header-only.
										self.callbackSuccess(s);
										return;
									}
								}
							}
						}
					}
					else if (self.status == BODY_CHUNKED)
					{
						if (self.bytes_remain == 0)
						{
							auto i = self.buf.find("\r\n");
							if (i == std::string::npos)
							{
								break;
							}
							try
							{
								self.bytes_remain = std::stoull(self.buf.substr(0, i), nullptr, 16);
							}
							catch (...)
							{
								self.callback(s, std::nullopt);
								return;
							}
							self.buf.erase(0, i + 2);
							if (self.bytes_remain == 0)
							{
								self.callbackSuccess(s);
								return;
							}
						}
						else
						{
							if (self.buf.size() < self.bytes_remain + 2)
							{
								break;
							}
							self.resp.body.append(self.buf.substr(0, self.bytes_remain));
							self.buf.erase(0, self.bytes_remain + 2);
							self.bytes_remain = 0;
						}
					}
					else if (self.status == BODY_LEN)
					{
						if (self.buf.size() < self.bytes_remain)
						{
							break;
						}
						self.resp.body = self.buf.substr(0, self.bytes_remain);
						//self.buf.erase(0, self.bytes_remain);
						self.callbackSuccess(s);
						return;
					}
					else if (self.status == BODY_CLOSE)
					{
						break;
					}
				}
				self.tick(s, std::move(cap));
			}, std::move(cap));
		}

		void callbackSuccess(Socket& s)
		{
			resp.decode();
			SOUP_IF_LIKELY (!HttpRequest::isChallengeResponse(resp))
			{
				callback(s, std::move(resp));
			}
			else
			{
				callback(s, std::nullopt);
			}
		}
	};

	void HttpRequest::recvResponse(Socket& s, void callback(Socket&, std::optional<HttpResponse>&&, Capture&&), Capture&& _cap)
	{
		Capture cap = HttpResponseReceiver(callback, std::move(_cap));
		cap.get<HttpResponseReceiver>().tick(s, std::move(cap));
	}
}

#endif
