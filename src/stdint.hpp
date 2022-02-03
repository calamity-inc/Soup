#pragma once

#include <cstdint>

#include "platform.hpp"

// Literally agreeing to disagree

#if SOUP_PLATFORM_WINDOWS
namespace soup
{
	using size_t = ::size_t;
	using ptrdiff_t = ::ptrdiff_t;
}
#else
namespace soup
{
	using size_t = std::size_t;
	using ptrdiff_t = std::ptrdiff_t;
}
#endif
