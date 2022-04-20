#pragma once

#include "base.hpp"

namespace soup
{
	struct os
	{
#if SOUP_WINDOWS
		static void stop();
#endif
	};
}
