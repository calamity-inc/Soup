#pragma once

#ifdef _WIN32
#define SOUP_PLATFORM_WINDOWS true
#ifdef _WIN64
#define SOUP_PLATFORM_BITS 64
#else
#define SOUP_PLATFORM_BITS 32
#endif
#else
#define SOUP_PLATFORM_WINDOWS false
#if defined(__x86_64__) || defined(__ppc64__)
#define SOUP_PLATFORM_BITS 64
#else
#define SOUP_PLATFORM_BITS 32
#endif
#endif

namespace soup
{
	struct platform
	{
		// os
		static constexpr bool windows = SOUP_PLATFORM_WINDOWS;

		// arch
		static constexpr auto bits = SOUP_PLATFORM_BITS;
	};
}
