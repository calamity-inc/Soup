#include "PointLight.hpp"

#include "Poly.hpp"

namespace soup
{
	float PointLight::getPointBrightness(const Vector3& p, const Poly& t) const
	{
		return getPointBrightness(p, t.getSurfaceNormal());
	}

	float PointLight::getPointBrightness(const Vector3& p, const Vector3& normal) const
	{
		return 1.0f - ((normal.dot(pos.lookAt(p).toDir()) + 1.0f) / 2.0f);
	}
}
