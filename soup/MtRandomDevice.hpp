#pragma once

#include "RandomDevice.hpp"

namespace soup
{
	struct MtRandomDevice : public RandomDevice
	{
		[[nodiscard]] uint64_t generate() final;
	};
}
