#pragma once

#include "base.hpp"

#if SOUP_CPP20
#include <bit>

#define SOUP_LITTLE_ENDIAN std::endian::native == std::endian::little
#else
#define SOUP_LITTLE_ENDIAN true
#endif
