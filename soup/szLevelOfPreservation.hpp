#pragma once

#include <cstdint>

namespace soup
{
	enum szLevelOfPreservation : uint8_t
	{
		NONE = 0,
		//CORRUPTED,
		LOSSY,
		LOSSLESS,
	};
}
