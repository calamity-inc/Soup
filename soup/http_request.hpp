#pragma once

#include "fwd.hpp"

#include "callback.hpp"
#include "mime_message.hpp"
#include "http_response.hpp"

namespace soup
{
	class http_request : public mime_message
	{
	public:
		std::string method;
	protected:
		std::string path;

	public:
		http_request(std::string&& method, std::string&& host, std::string&& path);
		http_request(std::string&& host, std::string&& path);

		[[nodiscard]] const std::string& getHost() const;

		void setPath(std::string&& path);
	private:
		void fixPath();
	public:
		void setPayload(std::string payload);

		void execute(callback<void(http_response&&)>&& on_success, callback<void()>&& on_fail = {}) const; // blocking
		void execute(callback<void(http_response&&)>&& on_success, callback<void()>&& on_fail, bool(*certchain_validator)(const certchain&, const std::string& server_name)) const; // blocking
	private:
		static void execute_tick(socket& s, std::string* resp);
	};
}
