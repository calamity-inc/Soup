#pragma once

#include <string>

namespace soup
{
	struct Uri
	{
		std::string scheme{};
		std::string host{};
		uint16_t port{};
		std::string user{};
		std::string pass{};
		std::string path{};
		std::string query{};
		std::string fragment{};

		Uri() = default;
		Uri(const char* url);
		Uri(std::string url);

		[[nodiscard]] std::string toString() const;

		[[nodiscard]] bool isHttp() const noexcept;
		[[nodiscard]] std::string getRequestPath() const;

		[[nodiscard]] bool isFile() const noexcept;
		[[nodiscard]] std::string getFilePath() const;

		[[nodiscard]] static std::string data(const char* mime_type, const std::string& contents);
	};
}
