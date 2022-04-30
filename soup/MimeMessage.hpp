#pragma once

#include <string>
#include <unordered_map>

namespace soup
{
	struct MimeMessage
	{
		std::unordered_map<std::string, std::string> header_fields{};
		std::string body{};

		MimeMessage() = default;
		MimeMessage(std::unordered_map<std::string, std::string>&& header_fields, std::string&& body = {});
		MimeMessage(const std::string& data);

		void loadMessage(const std::string& data);

		[[nodiscard]] std::string toString() const;
	};
}
