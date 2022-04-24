#pragma once

#include "JsonNode.hpp"

namespace soup
{
	struct JsonBool : public JsonNode
	{
		bool value;

		explicit JsonBool() noexcept
			: JsonNode(JSON_BOOL)
		{
		}

		explicit JsonBool(bool value) noexcept
			: JsonNode(JSON_BOOL), value(value)
		{
		}

		[[nodiscard]] std::string encode() const final
		{
			return value ? "true" : "false";
		}
	};
}
