#pragma once

#include "json_node.hpp"

namespace soup
{
	struct json_null : public json_node
	{
		explicit json_null() noexcept
			: json_node(JSON_NULL)
		{
		}

		[[nodiscard]] std::string encode() const final
		{
			return "null";
		}
	};
}
