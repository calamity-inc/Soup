#pragma once

#include <string>

#include "fwd.hpp"
#include "json_node_type.hpp"

namespace soup
{
	struct json_node
	{
		json_node_type type;

		explicit json_node(json_node_type type) noexcept
			: type(type)
		{
		}

		virtual ~json_node() = default;

		std::strong_ordering operator <=>(const json_node& b) const;
		bool operator ==(const json_node& b) const;
		bool operator !=(const json_node& b) const;
		bool operator <(const json_node& b) const;

		[[nodiscard]] virtual std::string encode() const = 0;
		[[nodiscard]] std::string encodePretty(const std::string& prefix = {}) const;

		[[nodiscard]] bool isArr() const noexcept;
		[[nodiscard]] bool isBool() const noexcept;
		[[nodiscard]] bool isFloat() const noexcept;
		[[nodiscard]] bool isInt() const noexcept;
		[[nodiscard]] bool isNull() const noexcept;
		[[nodiscard]] bool isObj() const noexcept;
		[[nodiscard]] bool isStr() const noexcept;

		// Type casts; will return nullptr if *this is a different type.
		[[nodiscard]] json_array* asArr() noexcept;
		[[nodiscard]] json_bool* asBool() noexcept;
		[[nodiscard]] json_float* asFloat() noexcept;
		[[nodiscard]] json_int* asInt() noexcept;
		[[nodiscard]] json_object* asObj() noexcept;
		[[nodiscard]] json_string* asStr() noexcept;

		// Reinterpret casts; no error considerations.
		[[nodiscard]] json_array& reinterpretAsArr() noexcept;
		[[nodiscard]] json_bool& reinterpretAsBool() noexcept;
		[[nodiscard]] json_float& reinterpretAsFloat() noexcept;
		[[nodiscard]] json_int& reinterpretAsInt() noexcept;
		[[nodiscard]] json_object& reinterpretAsObj() noexcept;
		[[nodiscard]] json_string& reinterpretAsStr() noexcept;
		[[nodiscard]] const json_array& reinterpretAsArr() const noexcept;
		[[nodiscard]] const json_bool& reinterpretAsBool() const noexcept;
		[[nodiscard]] const json_float& reinterpretAsFloat() const noexcept;
		[[nodiscard]] const json_int& reinterpretAsInt() const noexcept;
		[[nodiscard]] const json_object& reinterpretAsObj() const noexcept;
		[[nodiscard]] const json_string& reinterpretAsStr() const noexcept;
	};
}
