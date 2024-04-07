#pragma once

#include "base.hpp"
#if SOUP_WINDOWS
#include <Windows.h>

NAMESPACE_SOUP
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
