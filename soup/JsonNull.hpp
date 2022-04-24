#pragma once

#include "JsonNode.hpp"

namespace soup
{
	struct JsonNull : public JsonNode
	{
		explicit JsonNull() noexcept
			: JsonNode(JSON_NULL)
		{
		}

		[[nodiscard]] std::string encode() const final
		{
			return "null";
		}
	};
}
