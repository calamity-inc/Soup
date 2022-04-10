#pragma once

#include "json_node.hpp"

namespace soup
{
	struct json_int : public json_node
	{
		int64_t value;

		explicit json_int(int64_t value = 0) noexcept;

		[[nodiscard]] std::string encode() const final;
	};
}
