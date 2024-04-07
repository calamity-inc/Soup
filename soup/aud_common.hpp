#pragma once

#include "math.hpp"

#define HZ_TO_ANGVEL(x) ((x) * 2 * M_PI)

NAMESPACE_SOUP
{
	constexpr int AUD_SAMPLE_RATE = 44100;
	constexpr double AUD_TIME_STEP = (1.0 / (double)AUD_SAMPLE_RATE);
}
