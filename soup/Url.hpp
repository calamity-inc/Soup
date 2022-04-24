#pragma once

#include <string>

namespace soup
{
	struct Url
	{
		[[nodiscard]] static std::string data(const char* mime_type, const std::string& contents);
	};
}
