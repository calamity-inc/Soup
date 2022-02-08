#pragma once

#include "vector_base.hpp"

namespace soup
{
#pragma pack(push, 1)
	struct vector2 : public vector_base<2, vector2>
	{
		float x;
		float y;

		vector2() noexcept = default;

		constexpr vector2(float x, float y) noexcept
			: x(x), y(y)
		{
		}
	};
	static_assert(sizeof(vector2) == 2 * sizeof(float));
#pragma pack(pop)
}
