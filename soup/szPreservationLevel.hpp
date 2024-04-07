#pragma once

#include <cstdint>

#include "base.hpp"

NAMESPACE_SOUP
{
	enum szPreservationLevel : uint8_t
	{
		NONE = 0,
		CORRUPTED,
		LOSSY,
		LOSSLESS,
	};
}
