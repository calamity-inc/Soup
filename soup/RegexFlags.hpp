#pragma once

#include <cstdint>

namespace soup
{
	enum RegexFlags : uint16_t
	{
		RE_DOTALL = (1 << 0), // 's' - '.' also matches '\n'
		RE_MULTILINE = (1 << 1), // 'm' - '^' and '$' also match start and end of lines, respectively
		RE_DOLLAR_ENDONLY = (1 << 2), // 'D' - '$' only matches end of pattern, not '\n' - ignored if multi_line flag is set
	};
}
