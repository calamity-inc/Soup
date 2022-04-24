#pragma once

#include "fwd.hpp"
#include "Vector4.hpp"

namespace soup
{
#pragma pack(push, 1)
	class Quaternion : public Vector4
	{
	public:
		using Vector4::Vector4;

		Quaternion(float angle, const Vector3& axis) noexcept;

		void setAxisAngle(float degrees, float fX, float fY, float fZ) noexcept;

		[[nodiscard]] static Quaternion fromEuler(const Vector3& rot) noexcept; // ZXY
		[[nodiscard]] static Quaternion fromEulerZYX(const Vector3& rot) noexcept;
	private:
		[[nodiscard]] static Quaternion fromEuler(float rx, float ry, float rz) noexcept;

		[[nodiscard]] Quaternion invert() const noexcept;

		[[nodiscard]] Quaternion operator+ (const Quaternion& b) const noexcept;
		[[nodiscard]] Quaternion operator* (const Quaternion& b) const;
		[[nodiscard]] Quaternion operator*(float s) const noexcept;

		[[nodiscard]] bool operator== (const Quaternion& b) const noexcept;

		[[nodiscard]] bool isIdentity() const noexcept;
		void reset() noexcept;

	public:
		int normalise() noexcept; // Quaternions store scale as well as rotation, but usually we just want rotation, so we can normalize.

		// Creates a value for this Quaternion from spherical linear interpolation
		// t is the interpolation value from 0 to 1
		// if bReduceTo360 is true, the interpolation will take the shortest path for a 360 deg angle range (max delta rotation = 180 degrees)
		// if bReduceTo360 is false, the interpolation will take the shortest path for a 720 deg angle range (max delta rotation = 360 degrees)
		[[nodiscard]] static Quaternion slerp(const Quaternion& a, const Quaternion& b, float t, const bool bReduceTo360) noexcept;

		// linearly interpolate each component, then normalize the Quaternion
		// Unlike spherical interpolation, this does not rotate at a constant velocity,
		// although that's not necessarily a bad thing
		[[nodiscard]] static Quaternion nlerp(const Quaternion& a, const Quaternion& b, float t, const bool bReduceTo360) noexcept;

		void toMatrix(Matrix& m) const noexcept;
	private:
		void toMatrix(float mf[16]) const noexcept;

	public:
		void aimZAxis(const Vector3& P1, const Vector3& P2) noexcept; // Set this Quat to aim the Z-Axis along the vector from P1 to P2
	};
#pragma pack(pop)
}
