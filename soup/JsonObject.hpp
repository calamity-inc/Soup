#pragma once

#include "JsonNode.hpp"

#include <utility>
#include <vector>

#include "UniquePtr.hpp"

namespace soup
{
	struct JsonObject : public JsonNode
	{
		std::vector<std::pair<UniquePtr<JsonNode>, UniquePtr<JsonNode>>> children{};

		explicit JsonObject() noexcept;
		explicit JsonObject(const char*& c) noexcept;

		[[nodiscard]] std::string encode() const final;
		[[nodiscard]] std::string encodePretty(const std::string& prefix = {}) const;

		bool binaryEncode(Writer& w) const final;

		[[nodiscard]] bool contains(const JsonNode& k) const noexcept;
		[[nodiscard]] bool contains(std::string k) const noexcept;
		[[nodiscard]] JsonNode* at(const JsonNode& k) const noexcept;
		[[nodiscard]] JsonNode* at(std::string k) const noexcept;

		void add(UniquePtr<JsonNode>&& k, UniquePtr<JsonNode>&& v);
		void add(std::string k, UniquePtr<JsonNode>&& v) = delete;
		void add(std::string k, std::string v);
		void add(std::string k, const char* v);
		void add(std::string k, int8_t v);
		void add(std::string k, int16_t v);
		void add(std::string k, int32_t v);
		void add(std::string k, int64_t v);
		void add(std::string k, bool v);
	};
}
