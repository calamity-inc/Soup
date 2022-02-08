#pragma once

#include "vector_base.hpp"
#include "vector3_base.hpp"

namespace soup
{
#pragma pack(push, 1)
#pragma warning(push)
#pragma warning(disable: 26495) // uninitialised member variable
	struct vector3 : public vector3_base<vector3, vector_base<3, vector3>>
	{
		float x;
		float y;
		float z;

		vector3() noexcept = default;

		constexpr vector3(float x, float y, float z = 0.0f) noexcept
			: x(x), y(y), z(z)
		{
		}

		template <typename T>
		constexpr vector3(const T& b) noexcept
			: x(b.x), y(b.y), z(b.z)
		{
		}
	};
	static_assert(sizeof(vector3) == 3 * sizeof(float));
#pragma warning(pop)
#pragma pack(pop)
}
