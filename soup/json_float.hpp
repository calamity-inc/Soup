#pragma once

#include "json_node.hpp"

namespace soup
{
	struct json_float : public json_node
	{
		double value;

		explicit json_float(double value = 0.0) noexcept;

		[[nodiscard]] std::string encode() const final;
	};
}
