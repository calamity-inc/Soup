#pragma once

#include "RngInterface.hpp"

namespace soup
{
	struct DefaultRngInterface : public RngInterface
	{
		[[nodiscard]] uint64_t generate() final;
	};
}
