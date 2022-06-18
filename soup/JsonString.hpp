#pragma once

#include "JsonNode.hpp"

#include "string.hpp"

namespace soup
{
	struct JsonString : public JsonNode
	{
		std::string value{};

		explicit JsonString() noexcept;
		explicit JsonString(std::string&& value)noexcept;
		explicit JsonString(const char*& c);

		operator std::string& () noexcept
		{
			return value;
		}

		[[nodiscard]] std::string encode() const final;

		bool binaryEncode(Writer& w) const final;
	};
}
