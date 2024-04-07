#pragma once

#include "VectorBase.hpp"

NAMESPACE_SOUP
{
#pragma pack(push, 1)
	struct Vector4 : public VectorBase<4, Vector4>
	{
		float x;
		float y;
		float z;
		float w;

		Vector4() noexcept = default;

		Vector4(float x, float y, float z, float w) noexcept
			: x(x), y(y), z(z), w(w)
		{
		}
	};
	static_assert(sizeof(Vector4) == 4 * sizeof(float));
#pragma pack(pop)
}
