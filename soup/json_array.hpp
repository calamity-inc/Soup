#pragma once

#include "json_node.hpp"

#include "unique_ptr.hpp"
#include <vector>

namespace soup
{
	struct json_array : public json_node
	{
		std::vector<unique_ptr<json_node>> children{};

		explicit json_array() noexcept;
		explicit json_array(const char*& c);

		[[nodiscard]] std::string encode() const final;
		[[nodiscard]] std::string encodePretty(const std::string& prefix = {}) const;

		[[nodiscard]] json_node& at(size_t i);
	};
}
