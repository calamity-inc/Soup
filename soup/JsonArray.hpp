#pragma once

#include "JsonNode.hpp"

#include "UniquePtr.hpp"
#include <vector>

namespace soup
{
	struct JsonArray : public JsonNode
	{
		std::vector<UniquePtr<JsonNode>> children{};

		explicit JsonArray() noexcept;
		explicit JsonArray(const char*& c);

		[[nodiscard]] std::string encode() const final;
		[[nodiscard]] std::string encodePretty(const std::string& prefix = {}) const;

		bool binaryEncode(Writer& w) const final;

		[[nodiscard]] JsonNode& at(size_t i);
	};
}
