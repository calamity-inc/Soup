#pragma once

#include "json_node.hpp"
#include "unique_ptr.hpp"

namespace soup
{
	struct json
	{
		static void decode(unique_ptr<json_node>& out, const std::string& data);
		static void decode(unique_ptr<json_node>& out, const char*& c);

		[[nodiscard]] static unique_ptr<json_node> decodeForDedicatedVariable(const std::string& data);
		[[nodiscard]] static unique_ptr<json_node> decodeForDedicatedVariable(const char*& c);
	};
}
