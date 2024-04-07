#pragma once

#include "VectorBase.hpp"

NAMESPACE_SOUP
{
#pragma pack(push, 1)
	struct Vector2 : public VectorBase<2, Vector2>
	{
		float x;
		float y;

		Vector2() noexcept = default;

		constexpr Vector2(float x, float y) noexcept
			: x(x), y(y)
		{
		}
	};
	static_assert(sizeof(Vector2) == 2 * sizeof(float));
#pragma pack(pop)
}
