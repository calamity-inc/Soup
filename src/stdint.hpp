#pragma once

#include <cstdint>

#include "platform.hpp"

namespace soup
{
#if SOUP_PLATFORM_WINDOWS
	using size_t = ::size_t;
	using ptrdiff_t = ::ptrdiff_t;
#else
	using size_t = std::size_t;
	using ptrdiff_t = std::ptrdiff_t;
#endif

#if SOUP_PLATFORM_BITS == 64
	using halfintmax_t = int32_t;
	using halfsize_t = uint32_t;
#elif SOUP_PLATFORM_BITS == 32
	using halfintmax_t = int16_t;
	using halfsize_t = uint16_t;
#endif
}
