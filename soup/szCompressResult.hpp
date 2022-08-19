#pragma once

#include "BitString.hpp"
#include "szPreservationLevel.hpp"

namespace soup
{
	struct szCompressResult
	{
		BitString data{};
		szPreservationLevel preservation_level = NONE;
	};
}
