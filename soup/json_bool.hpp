#pragma once

#include "json_node.hpp"

namespace soup
{
	struct json_bool : public json_node
	{
		bool value;

		explicit json_bool() noexcept
			: json_node(JSON_BOOL)
		{
		}

		explicit json_bool(bool value) noexcept
			: json_node(JSON_BOOL), value(value)
		{
		}

		[[nodiscard]] std::string encode() const final
		{
			return value ? "true" : "false";
		}
	};
}
