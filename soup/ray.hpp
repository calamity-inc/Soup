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

		[[deprecated]] ray(const vector3& start, const vector3& rotation, float length) noexcept
			: start(start), end(start + (rotation.toDir() * length))
		{
		}

		[[nodiscard]] static ray withRot(const vector3& start, const vector3& rot, float len)
		{
			return withDir(start, rot.toDir(), len);
		}

		[[nodiscard]] static ray withDir(const vector3& start, const vector3& dir, float len)
		{
			return ray{ start, start + (dir * len) };
		}
	};
#pragma pack(pop)
}
