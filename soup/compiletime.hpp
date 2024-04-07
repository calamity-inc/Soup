#pragma once

#include "base.hpp"

NAMESPACE_SOUP
{
	struct compiletime
	{
		[[nodiscard]] static constexpr size_t strlen(const char* str)
		{
			size_t len = 0;
			while (*str)
			{
				++str;
				++len;
			}
			return len;
		}
	};
}
