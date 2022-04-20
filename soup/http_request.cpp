#include "http_request.hpp"

#include "obfus_string.hpp"
#include "scheduler.hpp"
#include "socket.hpp"
#include "unique_ptr.hpp"
#include "urlenc.hpp"

#include "deflate.hpp"
#include "joaat.hpp"

namespace soup
{
	http_request::http_request(std::string&& method, std::string&& host, std::string&& path)
		: mime_message({
			{obfus_string("Host"), std::move(host)},
			{obfus_string("User-Agent"), obfus_string("Mozilla/5.0 (compatible; Soup)")},
			{obfus_string("Connection"), obfus_string("close")},
			{obfus_string("Accept-Encoding"), obfus_string("gzip, deflate")},
		}), method(std::move(method)), path(std::move(path))
	{
		fixPath();
	}

	http_request::http_request(std::string&& host, std::string&& path)
		: http_request(obfus_string("GET"), std::move(host), std::move(path))
	{
	}

	const std::string& http_request::getHost() const
	{
		return header_fields.at(obfus_string("Host"));
	}

	void http_request::setPath(std::string&& path)
	{
		this->path = std::move(path);
		fixPath();
	}

	void http_request::fixPath()
	{
		if (auto it = path.cbegin(); it == path.cend() || *it != '/')
		{
			path.insert(0, 1, '/');
		}
	}

	void http_request::setPayload(std::string payload)
	{
		header_fields.emplace(obfus_string("Content-Length"), std::to_string(payload.size()));
		body = std::move(payload);
	}

	struct capture_http_request_execute
	{
		const http_request* req;
		std::string* resp;
	};

	void http_request::execute(callback<void(http_response&&)>&& on_success, callback<void()>&& on_fail) const
	{
		return execute(std::move(on_success), std::move(on_fail), &socket::trustAllCertchainsWithNoChecksWhatsoever_ThisIsNotAJoke_IfYouCareYouShouldLookIntoThis);
	}

	void http_request::execute(callback<void(http_response&&)>&& on_success, callback<void()>&& on_fail, bool(*certchain_validator)(const certchain&, const std::string& server_name)) const
	{
		auto sock = make_unique<socket>();
		auto resp = make_unique<std::string>();
		const auto& host = getHost();
		if (sock->connect(host, 443))
		{
			scheduler sched{};
			sched.addSocket(std::move(sock)).enableCryptoClient(host, [](socket& s, capture&& _cap)
			{
				auto& cap = _cap.get<capture_http_request_execute>();

				std::string data{};
				data.append(cap.req->method);
				data.push_back(' ');
				data.append(urlenc::encodePathWithQuery(cap.req->path));
				data.append(obfus_string(" HTTP/1.0").str());
				data.append("\r\n");
				data.append(cap.req->toString());

				s.send(data);

				execute_tick(s, cap.resp);
			}, capture_http_request_execute{ this, resp.get() }, certchain_validator);
			sched.run();
		}
		if (!resp->empty())
		{
			http_response res = std::move(*resp);
			if (auto enc = res.header_fields.find(obfus_string("Content-Encoding")); enc != res.header_fields.end())
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

	void http_request::execute_tick(socket& s, std::string* resp)
	{
		s.recv([](socket& s, std::string&& app, capture&& cap)
		{
			std::string* resp = cap.get<std::string*>();
			resp->append(std::move(app));
			execute_tick(s, resp);
		}, resp);
	}
}
