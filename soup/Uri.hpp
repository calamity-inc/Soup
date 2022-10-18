#pragma once

#include <string>

namespace soup
{
	struct Uri
	{
		std::string scheme{};
		std::string host{};
		int16_t port{};
		std::string user{};
		std::string pass{};
		std::string path{};
		std::string query{};
		std::string fragment{};

		Uri() = default;
		Uri(std::string url);

		[[nodiscard]] std::string toString() const;

		[[nodiscard]] std::string getRequestPath() const;

		[[nodiscard]] static std::string data(const char* mime_type, const std::string& contents);
	};
}
