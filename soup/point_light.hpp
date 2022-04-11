#pragma once

#include "fwd.hpp"
#include "vector3.hpp"

namespace soup
{
	struct point_light
	{
		vector3 pos;

		[[nodiscard]] float getPointBrightness(const vector3& p, const poly& t) const;
		[[nodiscard]] float getPointBrightness(const vector3& p, const vector3& normal) const;
	};
}
