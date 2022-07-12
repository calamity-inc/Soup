#pragma once

#include "TreeNode.hpp"

#include "base.hpp"
#include "fwd.hpp"

#include "JsonNodeType.hpp"
#include <string>

namespace soup
{
	struct JsonNode : public TreeNode
	{
		JsonNodeType type;

		explicit JsonNode(JsonNodeType type) noexcept
			: type(type)
		{
		}

#if SOUP_CPP20
		std::strong_ordering operator <=>(const JsonNode& b) const;
#endif
		bool operator ==(const JsonNode& b) const;
		bool operator !=(const JsonNode& b) const;
		bool operator <(const JsonNode& b) const;

		[[nodiscard]] virtual std::string encode() const = 0;
		[[nodiscard]] std::string encodePretty(const std::string& prefix = {}) const;

		virtual bool binaryEncode(Writer& w) const; // specific to soup

		[[nodiscard]] bool isArr() const noexcept;
		[[nodiscard]] bool isBool() const noexcept;
		[[nodiscard]] bool isFloat() const noexcept;
		[[nodiscard]] bool isInt() const noexcept;
		[[nodiscard]] bool isNull() const noexcept;
		[[nodiscard]] bool isObj() const noexcept;
		[[nodiscard]] bool isStr() const noexcept;

		// Type casts; will return nullptr if *this is a different type.
		[[nodiscard]] JsonArray* asArr() noexcept;
		[[nodiscard]] JsonBool* asBool() noexcept;
		[[nodiscard]] JsonFloat* asFloat() noexcept;
		[[nodiscard]] JsonInt* asInt() noexcept;
		[[nodiscard]] JsonObject* asObj() noexcept;
		[[nodiscard]] JsonString* asStr() noexcept;

		// Reinterpret casts; no error considerations.
		[[nodiscard]] JsonArray& reinterpretAsArr() noexcept;
		[[nodiscard]] JsonBool& reinterpretAsBool() noexcept;
		[[nodiscard]] JsonFloat& reinterpretAsFloat() noexcept;
		[[nodiscard]] JsonInt& reinterpretAsInt() noexcept;
		[[nodiscard]] JsonObject& reinterpretAsObj() noexcept;
		[[nodiscard]] JsonString& reinterpretAsStr() noexcept;
		[[nodiscard]] const JsonArray& reinterpretAsArr() const noexcept;
		[[nodiscard]] const JsonBool& reinterpretAsBool() const noexcept;
		[[nodiscard]] const JsonFloat& reinterpretAsFloat() const noexcept;
		[[nodiscard]] const JsonInt& reinterpretAsInt() const noexcept;
		[[nodiscard]] const JsonObject& reinterpretAsObj() const noexcept;
		[[nodiscard]] const JsonString& reinterpretAsStr() const noexcept;
	};
}
