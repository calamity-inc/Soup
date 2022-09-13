#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#define M_TAU (M_PI * 2.0)

#define DEG_TO_RAD(deg) ((deg) * (float(M_PI) / 180.0f))
#define DEG_TO_HALF_RAD(deg) ((deg) * (float(M_PI) / 360.0f))

namespace soup
{
	template <typename T = float>
	[[nodiscard]] constexpr T lerp(T a, T b, float t)
	{
		return (T)(a + (b - a) * t);
	}

	template <typename T>
	[[nodiscard]] constexpr T pow(T a, T b) // b must be >= 0
	{
		if (b == 0)
		{
			return 1;
		}
		T res = a;
		while (--b)
		{
			res *= a;
		}
		return res;
	}
}
