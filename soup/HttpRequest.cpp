#include "HttpRequest.hpp"

#if !SOUP_WASM

#include "deflate.hpp"
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
		return execute(&Socket::trustAllCertchainsWithNoChecksWhatsoever_ThisIsNotAJoke_IfYouCareYouShouldLookIntoThis);
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
					cap.req->execute_send(s);
					execute_tick(s, cap.resp);
				}, CaptureHttpRequestExecute{ this, &resp }, certchain_validator);
			}
			else
			{
				execute_send(s);
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
					else
					{
						// If no Transfer-Encoding, we'd have Content-Length in HTTP/1.1, or "Connection: close" in HTTP/1.0.
					}

					if (auto enc = res.header_fields.find(ObfusString("Content-Encoding")); enc != res.header_fields.end())
					{
						auto enc_joaat = joaat::hash(enc->second);
						switch (enc_joaat)
						{
						case joaat::hash("gzip"):
						case joaat::hash("deflate"):
							res.body = deflate::decompress(res.body).decompressed;
							break;
						}
					}

					if (res.body.find(ObfusString(R"(href="https://www.cloudflare.com?utm_source=challenge)").str()) == std::string::npos)
					{
						return std::optional<HttpResponse>(std::move(res));
					}
				}
			}
		}
		return {};
	}

	std::string HttpRequest::getDataToSend() const
	{
		std::string data{};
		data.append(method);
		data.push_back(' ');
		data.append(path_is_encoded ? path : urlenc::encodePathWithQuery(path));
		data.append(ObfusString(" HTTP/1.1").str());
		data.append("\r\n");
		data.append(toString());
		return data;
	}

	void HttpRequest::execute_send(Socket& s) const
	{
		s.send(getDataToSend());
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
}

#endif
