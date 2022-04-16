#pragma once

#include "fwd.hpp"
#include "vector4.hpp"

namespace soup
{
#pragma pack(push, 1)
	class quaternion : public vector4
	{
	public:
		using vector4::vector4;

		quaternion(float angle, const vector3& axis) noexcept;

		void setAxisAngle(float degrees, float fX, float fY, float fZ) noexcept;

		[[nodiscard]] static quaternion fromEuler(const vector3& rot) noexcept; // ZXY
		[[nodiscard]] static quaternion fromEulerZYX(const vector3& rot) noexcept;
	private:
		[[nodiscard]] static quaternion fromEuler(float rx, float ry, float rz) noexcept;

		[[nodiscard]] quaternion invert() const noexcept;

		[[nodiscard]] quaternion operator+ (const quaternion& b) const noexcept;
		[[nodiscard]] quaternion operator* (const quaternion& b) const;
		[[nodiscard]] quaternion operator*(float s) const noexcept;

		[[nodiscard]] bool operator== (const quaternion& b) const noexcept;

		[[nodiscard]] bool isIdentity() const noexcept;
		void reset() noexcept;

	public:
		int normalise() noexcept; // Quaternions store scale as well as rotation, but usually we just want rotation, so we can normalize.

		// Creates a value for this Quaternion from spherical linear interpolation
		// t is the interpolation value from 0 to 1
		// if bReduceTo360 is true, the interpolation will take the shortest path for a 360 deg angle range (max delta rotation = 180 degrees)
		// if bReduceTo360 is false, the interpolation will take the shortest path for a 720 deg angle range (max delta rotation = 360 degrees)
		[[nodiscard]] static quaternion slerp(const quaternion& a, const quaternion& b, float t, const bool bReduceTo360) noexcept;

		// linearly interpolate each component, then normalize the Quaternion
		// Unlike spherical interpolation, this does not rotate at a constant velocity,
		// although that's not necessarily a bad thing
		[[nodiscard]] static quaternion nlerp(const quaternion& a, const quaternion& b, float t, const bool bReduceTo360) noexcept;

		void toMatrix(matrix& m) const noexcept;
	private:
		void toMatrix(float mf[16]) const noexcept;

	public:
		void aimZAxis(const vector3& P1, const vector3& P2) noexcept; // Set this Quat to aim the Z-Axis along the vector from P1 to P2
	};
#pragma pack(pop)
}
