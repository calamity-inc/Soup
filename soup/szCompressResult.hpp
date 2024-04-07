#pragma once

#include "BitString.hpp"
#include "szPreservationLevel.hpp"

NAMESPACE_SOUP
{
	struct szCompressResult
	{
		BitString data{};
		szPreservationLevel preservation_level = NONE;
	};
}
