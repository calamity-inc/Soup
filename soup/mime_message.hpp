#pragma once

#include <string>
#include <unordered_map>

namespace soup
{
	struct mime_message
	{
		std::unordered_map<std::string, std::string> header_fields{};
		std::string body{};

		mime_message() = default;
		mime_message(std::unordered_map<std::string, std::string>&& header_fields, std::string&& body = {});
		mime_message(const std::string& data);

		[[nodiscard]] std::string toString() const;
	};
}
