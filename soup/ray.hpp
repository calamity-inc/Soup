#pragma once

#include "vector3.hpp"

namespace soup
{
#pragma pack(push, 1)
	struct ray
	{
		vector3 start;
		vector3 end;

		ray(const vector3& start, const vector3& end) noexcept
			: start(start), end(end)
		{
		}

		ray(const vector3& start, const vector3& rotation, float length) noexcept
			: start(start), end(start + (rotation.getUnitVector() * length))
		{
		}
	};
#pragma pack(pop)
}
