#pragma once

#include "math.hpp"

namespace soup
{
	template <typename T, typename Base /*= VectorBase<3, T>*/>
	struct Vector3Base : public Base
	{
		template <typename T2>
		[[nodiscard]] T crossProduct(const T2& b) const
		{
			return T(
				((const T*)this)->y * b.z - ((const T*)this)->z * b.y,
				((const T*)this)->z * b.x - ((const T*)this)->x * b.z,
				((const T*)this)->x * b.y - ((const T*)this)->y * b.x
			);
		}

		template <typename T2>
		[[nodiscard]] float distanceTopdown(const T2& b) const
		{
			return sqrt(pow2(b.x - ((const T*)this)->x) + pow2(b.y - ((const T*)this)->y));
		}

		[[nodiscard]] T toRot() const noexcept
		{
			const float yaw = -atan2f(((const T*)this)->x, ((const T*)this)->y) / ((float)M_PI) * 180.0f;
			const float pitch = asinf(((const T*)this)->z / Base::magnitude()) / ((float)M_PI) * 180.0f;
			return T{
				isnan(pitch) ? 0.0f : pitch,
				0.0f,
				yaw
			};
		}

		template <typename T2>
		[[nodiscard]] T rotate(const T2& rot) const noexcept
		{
			auto as_rot = toRot();
			as_rot += rot;
			return (as_rot.toDir() * Base::magnitude());
		}

		//template <typename T2, typename T3>
		//[[nodiscard]] T rotateAround(const T2& origin, const T3& rot) const noexcept
		//{
		//	// C++ sucks, this won't compile.
		//	//auto as_rot = origin.lookAt(*this);
		//	auto as_rot = (origin - *this).toRot();
		//
		//	as_rot += rot;
		//	return origin + (as_rot.toDir() * origin.distance(*this));
		//}

		// Rotates by the 3x3 rotation portion of a 4x4 matrix
		[[nodiscard]] T rotateByMatrix(const float m[16]) const noexcept
		{
			return T{
				((const T*)this)->x * m[0] + ((const T*)this)->y * m[4] + ((const T*)this)->z * m[8],
				((const T*)this)->x * m[1] + ((const T*)this)->y * m[5] + ((const T*)this)->z * m[9],
				((const T*)this)->x * m[2] + ((const T*)this)->y * m[6] + ((const T*)this)->z * m[10]
			};
		}

		// position vectors

		template <typename T2>
		[[nodiscard]] T2 lookAtDir(const T2& b) const noexcept
		{
			return (b - *(const T*)this);
		}

		template <typename T2>
		[[nodiscard]] T2 lookAt(const T2& b) const noexcept
		{
			return lookAtDir(b).toRot();
		}

		// rotation vectors

		[[nodiscard]] T toDir() const noexcept
		{
			const float yaw_radians = DEG_TO_RAD(((const T*)this)->z);
			const float pitch_radians = DEG_TO_RAD(((const T*)this)->x) * -1.0f;
			return T{
				cosf(pitch_radians) * sinf(yaw_radians) * -1.0f,
				cosf(pitch_radians) * cosf(yaw_radians),
				sinf(pitch_radians) * -1.0f
			};
		}

		[[nodiscard]] float toDirZ() const noexcept
		{
			const float pitch_radians = DEG_TO_RAD(((const T*)this)->x) * -1.0f;
			return sinf(pitch_radians) * -1.0f;
		}

		[[nodiscard]] T toDirNoZ() const noexcept
		{
			const float yaw_radians = DEG_TO_RAD(((const T*)this)->z);
			const float pitch_radians = DEG_TO_RAD(((const T*)this)->x) * -1.0f;
			return T{
				cosf(pitch_radians) * sinf(yaw_radians) * -1.0f,
				cosf(pitch_radians) * cosf(yaw_radians),
				0.0f
			};
		}

		[[nodiscard]] float heading() const noexcept
		{
			return ((((const T*)this)->z < 0.0) ? (((const T*)this)->z + 360.0f) : ((const T*)this)->z);
		}
	};
}
