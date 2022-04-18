#pragma once

#include <string>

namespace soup
{
	struct url
	{
		[[nodiscard]] static std::string data(const char* mime_type, const std::string& contents);
	};
}
