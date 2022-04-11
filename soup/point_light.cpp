#include "point_light.hpp"

#include "poly.hpp"

namespace soup
{
	float point_light::getPointBrightness(const vector3& p, const poly& t) const
	{
		return getPointBrightness(p, t.getSurfaceNormal());
	}

	float point_light::getPointBrightness(const vector3& p, const vector3& normal) const
	{
		return 1.0f - ((normal.dot(pos.lookAt(p).toDir()) + 1.0f) / 2.0f);
	}
}
