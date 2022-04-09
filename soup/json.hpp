#pragma once

#include "json_node.hpp"
#include "unique_ptr.hpp"

namespace soup
{
	struct json
	{
		[[nodiscard]] static unique_ptr<json_node> decode(const std::string& data);
		[[nodiscard]] static unique_ptr<json_node> decode(const char*& c);
	};
}
