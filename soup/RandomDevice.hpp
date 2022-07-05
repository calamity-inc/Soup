#pragma once

#include <cstdint>

namespace soup
{
	struct RandomDevice
	{
		[[nodiscard]] virtual uint64_t generate() = 0;
	};
}
