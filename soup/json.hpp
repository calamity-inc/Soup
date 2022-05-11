#pragma once

#include "JsonNode.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	struct json
	{
		static void decode(UniquePtr<JsonNode>& out, const std::string& data);
		static void decode(UniquePtr<JsonNode>& out, const char*& c);

		[[nodiscard]] static UniquePtr<JsonNode> decodeForDedicatedVariable(const std::string& data);
		[[nodiscard]] static UniquePtr<JsonNode> decodeForDedicatedVariable(const char*& c);
	};
}
