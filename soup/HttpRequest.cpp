#include "HttpRequest.hpp"

#include "ObfusString.hpp"
#include "Scheduler.hpp"
#include "Socket.hpp"
#include "UniquePtr.hpp"
#include "urlenc.hpp"

#include "deflate.hpp"
#include "joaat.hpp"

namespace soup
{
	HttpRequest::HttpRequest(std::string&& method, std::string&& host, std::string&& path)
		: MimeMessage({
			{ObfusString("Host"), std::move(host)},
			{ObfusString("User-Agent"), ObfusString("Mozilla/5.0 (compatible; Soup)")},
			{ObfusString("Connection"), ObfusString("close")},
			{ObfusString("Accept-Encoding"), ObfusString("deflate, gzip")},
		}), method(std::move(method)), path(std::move(path))
	{
		fixPath();
	}

	HttpRequest::HttpRequest(std::string&& host, std::string&& path)
		: HttpRequest(ObfusString("GET"), std::move(host), std::move(path))
	{
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
		if (auto it = path.cbegin(); it == path.cend() || *it != '/')
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

	void HttpRequest::execute(Callback<void(HttpResponse&&)>&& on_success, Callback<void()>&& on_fail) const
	{
		return execute(std::move(on_success), std::move(on_fail), &Socket::trustAllCertchainsWithNoChecksWhatsoever_ThisIsNotAJoke_IfYouCareYouShouldLookIntoThis);
	}

	void HttpRequest::execute(Callback<void(HttpResponse&&)>&& on_success, Callback<void()>&& on_fail, bool(*certchain_validator)(const Certchain&, const std::string& server_name)) const
	{
		auto sock = make_unique<Socket>();
		auto resp = make_unique<std::string>();
		const auto& host = getHost();
		if (sock->connect(host, 443))
		{
			Scheduler sched{};
			sched.addSocket(std::move(sock)).enableCryptoClient(host, [](Socket& s, Capture&& _cap)
			{
				auto& cap = _cap.get<CaptureHttpRequestExecute>();

				std::string data{};
				data.append(cap.req->method);
				data.push_back(' ');
				data.append(urlenc::encodePathWithQuery(cap.req->path));
				data.append(ObfusString(" HTTP/1.0").str());
				data.append("\r\n");
				data.append(cap.req->toString());

				s.send(data);

				execute_tick(s, cap.resp);
			}, CaptureHttpRequestExecute{ this, resp.get() }, certchain_validator);
			sched.run();
		}
		if (!resp->empty())
		{
			HttpResponse res = std::move(*resp);
			if (auto enc = res.header_fields.find(ObfusString("Content-Encoding")); enc != res.header_fields.end())
			{
				auto enc_joaat = joaat::hash(enc->second);
				if (enc_joaat == joaat::hash("gzip")
					|| enc_joaat == joaat::hash("deflate")
					)
				{
					res.body = deflate::decompress(res.body).decompressed;
				}
			}
			on_success(std::move(res));
		}
		else
		{
			if (on_fail)
			{
				on_fail();
			}
		}
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
