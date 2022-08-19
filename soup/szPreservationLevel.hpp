#pragma once

#include <cstdint>

namespace soup
{
	enum szPreservationLevel : uint8_t
	{
		NONE = 0,
		CORRUPTED,
		LOSSY,
		LOSSLESS,
	};
}
