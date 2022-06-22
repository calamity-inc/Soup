#include "MimeMessage.hpp"

#include "MimeType.hpp"
#include "string.hpp"

namespace soup
{
	MimeMessage::MimeMessage(std::unordered_map<std::string, std::string>&& header_fields, std::string&& body)
		: header_fields(std::move(header_fields)), body(std::move(body))
	{
	}

	MimeMessage::MimeMessage(const std::string& data)
	{
		loadMessage(data);
	}

	void MimeMessage::setContentLength()
	{
		header_fields.emplace("Content-Length", std::to_string(body.size()));
	}

	void MimeMessage::setContentType()
	{
		if (body.length() > 4
			&& body.substr(0, 4) == "\x89PNG"
			)
		{
			header_fields.emplace("Content-Type", MimeType::IMAGE_PNG);
			return;
		}

		if (body.find("<body>") != std::string::npos)
		{
			header_fields.emplace("Content-Type", MimeType::TEXT_HTML);
			return;
		}

		header_fields.emplace("Content-Type", MimeType::TEXT_PLAIN);
	}

	void MimeMessage::loadMessage(const std::string& data)
	{
		auto headers_end = data.find("\r\n\r\n");
		if (headers_end == std::string::npos)
		{
			headers_end = data.size();
		}

		auto body_begin = headers_end + 4;
		if (body_begin < data.size())
		{
			body = data.substr(body_begin);
		}
		else
		{
			body_begin = data.size();
		}

		auto header = data.substr(0, headers_end);
		body = data.substr(body_begin);

		for (const auto& line : string::explode(header, "\r\n"))
		{
			if (auto key_offset = line.find(": "); key_offset != std::string::npos)
			{
				header_fields.emplace(line.substr(0, key_offset), line.substr(key_offset + 2));
			}
		}
	}

	std::string MimeMessage::toString() const
	{
		std::string res{};
		for (const auto& field : header_fields)
		{
			res.append(field.first).append(": ").append(field.second).append("\r\n");
		}
		res.append("\r\n");
		res.append(body);
		return res;
	}
}
