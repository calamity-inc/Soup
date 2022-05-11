#pragma once

#include "JsonNode.hpp"

#include "DerivableMap.hpp"

namespace soup
{
	struct JsonObject : public JsonNode
	{
		DerivableMap<JsonNode, UniquePtr<JsonNode>> children{};

		explicit JsonObject() noexcept;
		explicit JsonObject(const char*& c) noexcept;

		[[nodiscard]] std::string encode() const final;
		[[nodiscard]] std::string encodePretty(const std::string& prefix = {}) const;

		bool binaryEncode(Writer& w) const final;

		[[nodiscard]] bool contains(const JsonNode& k);
		[[nodiscard]] bool contains(std::string k);
		[[nodiscard]] JsonNode& at(const JsonNode& k);
		[[nodiscard]] JsonNode& at(std::string k);

		void add(UniquePtr<JsonNode>&& k, UniquePtr<JsonNode>&& v);
		void add(std::string k, std::string v);
		void add(std::string k, int64_t v);
	};
}
