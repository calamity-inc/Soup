#pragma once

#include "algRng.hpp"

namespace soup
{
	struct algMtRng : public algRng
	{
		[[nodiscard]] uint64_t generate() final;
	};
}
