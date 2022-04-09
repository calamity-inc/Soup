#pragma once

#include "json_node.hpp"

#include "string.hpp"

namespace soup
{
	struct json_string : public json_node
	{
		std::string value{};

		explicit json_string() noexcept;
		explicit json_string(std::string&& value)noexcept;
		explicit json_string(const char*& c);

		[[nodiscard]] std::string encode() const final;
	};
}
