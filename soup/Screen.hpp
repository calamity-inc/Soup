#pragma once

#include "base.hpp"
#if SOUP_WINDOWS
#include <Windows.h>

namespace soup
{
	struct Screen
	{
		static int getPrimaryScreenWidth()
		{
			return GetSystemMetrics(SM_CXSCREEN);
		}

		static int getPrimaryScreenHeight()
		{
			return GetSystemMetrics(SM_CYSCREEN);
		}
	};
}
#endif
