#pragma once

#include <cstdint>

namespace soup
{
	enum RegexFlags : uint16_t
	{
		RE_MULTILINE = (1 << 0), // 'm' - '^' and '$' also match start and end of lines, respectively
		RE_DOTALL = (1 << 1), // 's' - '.' also matches '\n'
		RE_UNICODE = (1 << 2), // 'u' - Treat pattern and strings-to-match as UTF-8 instead of binary data
		RE_UNGREEDY = (1 << 3), // 'U' - Quantifiers become lazy by default and are instead made greedy by a trailing '?'
		RE_DOLLAR_ENDONLY = (1 << 4), // 'D' - '$' only matches end of pattern, not '\n' - ignored if multi_line flag is set
	};
}
