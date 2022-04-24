#pragma once

#include "Vector3.hpp"

namespace soup
{
#pragma pack(push, 1)
	struct Ray
	{
		Vector3 start;
		Vector3 end;

		Ray(const Vector3& start, const Vector3& end) noexcept
			: start(start), end(end)
		{
		}

		[[nodiscard]] static Ray withRot(const Vector3& start, const Vector3& rot, float len)
		{
			return withDir(start, rot.toDir(), len);
		}

		[[nodiscard]] static Ray withDir(const Vector3& start, const Vector3& dir, float len)
		{
			return Ray{ start, start + (dir * len) };
		}
	};
#pragma pack(pop)
}
