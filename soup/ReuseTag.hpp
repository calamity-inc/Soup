#pragma once

#include <string>

namespace soup
{
	struct ReuseTag
	{
		std::string host;
		bool is_busy = true;
	};
}
