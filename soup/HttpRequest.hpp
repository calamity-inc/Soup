#pragma once

#include "base.hpp"
#if !SOUP_WASM
#include "fwd.hpp"

#include <optional>

#include "Callback.hpp"
#include "MimeMessage.hpp"
#include "HttpResponse.hpp"

namespace soup
{
	class HttpRequest : public MimeMessage
	{
	public:
		bool use_tls = true;
		uint16_t port = 443;
		std::string method{};
		std::string path{};
		bool path_is_encoded = false;

		HttpRequest() = default;
		HttpRequest(std::string method, std::string host, std::string path);
		HttpRequest(std::string host, std::string path);
		HttpRequest(const Uri& uri);

		[[nodiscard]] const std::string& getHost() const;

		void setPath(std::string&& path);
	private:
		void fixPath();
	public:
		void setPayload(std::string payload);

		[[nodiscard]] std::optional<HttpResponse> execute() const; // blocking
		[[nodiscard]] std::optional<HttpResponse> execute(bool(*certchain_validator)(const X509Certchain&, const std::string& server_name)) const; // blocking
	private:
		void execute_send(Socket& s) const;
		static void execute_tick(Socket& s, std::string* resp);
	};
}
#endif
