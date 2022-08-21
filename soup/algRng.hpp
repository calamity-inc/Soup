#pragma once

#include <cstdint>

namespace soup
{
	struct algRng
	{
		[[nodiscard]] virtual uint64_t generate() = 0;
	};
}
