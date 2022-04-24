#pragma once

#include "fwd.hpp"
#include "Vector3.hpp"

namespace soup
{
	struct PointLight
	{
		Vector3 pos;

		[[nodiscard]] float getPointBrightness(const Vector3& p, const Poly& t) const;
		[[nodiscard]] float getPointBrightness(const Vector3& p, const Vector3& normal) const;
	};
}
