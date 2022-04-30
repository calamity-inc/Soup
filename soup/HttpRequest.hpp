#pragma once

#include "fwd.hpp"

#include "Callback.hpp"
#include "MimeMessage.hpp"
#include "HttpResponse.hpp"

namespace soup
{
	class HttpRequest : public MimeMessage
	{
	public:
		std::string method{};
		std::string path{};

		HttpRequest() = default;
		HttpRequest(std::string&& method, std::string&& host, std::string&& path);
		HttpRequest(std::string&& host, std::string&& path);

		[[nodiscard]] const std::string& getHost() const;

		void setPath(std::string&& path);
	private:
		void fixPath();
	public:
		void setPayload(std::string payload);

		void execute(Callback<void(HttpResponse&&)>&& on_success, Callback<void()>&& on_fail = {}) const; // blocking
		void execute(Callback<void(HttpResponse&&)>&& on_success, Callback<void()>&& on_fail, bool(*certchain_validator)(const Certchain&, const std::string& server_name)) const; // blocking
	private:
		static void execute_tick(Socket& s, std::string* resp);
	};
}
