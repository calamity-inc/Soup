#pragma once

#include "VectorBase.hpp"
#include "Vector3Base.hpp"

namespace soup
{
#pragma pack(push, 1)
#pragma warning(push)
#pragma warning(disable: 26495) // uninitialised member variable
	struct Vector3 : public Vector3Base<Vector3, VectorBase<3, Vector3>>
	{
		float x;
		float y;
		float z;

		Vector3() noexcept = default;

		constexpr Vector3(float x, float y, float z = 0.0f) noexcept
			: x(x), y(y), z(z)
		{
		}

		template <typename T>
		constexpr Vector3(const T& b) noexcept
			: x(b.x), y(b.y), z(b.z)
		{
		}
	};
	static_assert(sizeof(Vector3) == 3 * sizeof(float));
#pragma warning(pop)
#pragma pack(pop)
}
