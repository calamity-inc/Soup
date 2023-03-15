#pragma once

#include <string>
#include <unordered_map>

namespace soup
{
	// This is \r\n line endings land.
	struct MimeMessage
	{
		std::unordered_map<std::string, std::string> header_fields{};
		std::string body{};

		MimeMessage() = default;
		MimeMessage(std::unordered_map<std::string, std::string>&& header_fields, std::string&& body = {});
		MimeMessage(const std::string& data);

		void setContentLength();
		void setContentType();

		void loadMessage(const std::string& data);
		void addHeader(const std::string& line);

		[[nodiscard]] std::string toString() const;

		[[nodiscard]] std::string getCanonicalisedBody(bool relaxed) const;
	};
}
