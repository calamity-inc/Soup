#pragma once

#include "BitString.hpp"
#include "szLevelOfPreservation.hpp"

namespace soup
{
	struct szCompressResult
	{
		BitString data{};
		szLevelOfPreservation level_of_preservation = NONE;
	};
}
