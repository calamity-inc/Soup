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

		[[nodiscard]] std::string encode() const final;
	};
}
