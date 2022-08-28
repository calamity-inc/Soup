#pragma once

#include "base.hpp"

#if SOUP_CPP20
#include <bit>
#endif

#undef LITTLE_ENDIAN
#undef BIG_ENDIAN

namespace soup
{
	enum Endian : bool
	{
		LITTLE_ENDIAN = true,
		BIG_ENDIAN = false,
#if SOUP_CPP20
		NATIVE_ENDIAN = (std::endian::native == std::endian::little)
#else
		NATIVE_ENDIAN = true
#endif
	};
}
