#pragma once

#include <cstdint>

namespace soup
{
	enum RegexFlags : uint16_t
	{
		RE_DOTALL = (1 << 0), // 's' - '.' also matches '\n'
		RE_MULTILINE = (1 << 1), // 'm' - '^' and '$' also match start and end of lines, respectively
		RE_DOLLAR_ENDONLY = (1 << 2), // 'D' - '$' only matches end of pattern, not '\n' - ignored if multi_line flag is set
		RE_UNICODE = (1 << 3), // 'u' - Treat pattern and strings-to-match as UTF-8 instead of binary data
		RE_UNGREEDY = (1 << 4), // 'U' - Quantifiers become lazy by default and are instead made greedy by a trailing '?'
	};
}
