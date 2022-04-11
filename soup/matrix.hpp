#pragma once

#include "fwd.hpp"

namespace soup
{
#pragma pack(push, 1)
#pragma warning(push)
#pragma warning(disable: 26495) // uninitialised member variable
	class matrix
	{
	public:
		float mf[16];

		matrix() noexcept;
		matrix(const vector3& pos, const vector3& rot) noexcept;

		void reset() noexcept; // set to identity

		[[nodiscard]] matrix operator* (const matrix& InM) const noexcept;
		void operator*= (const matrix& InM) noexcept;
		[[nodiscard]] vector3 operator* (const vector3& Point) const noexcept;

		void resetRotation() noexcept;
		void setRotation(const vector3& rot) noexcept;
		void rotate(const vector3& rot) noexcept;
		void rotateCCW(const vector3& rot) noexcept;
		void rotateX(float fDegrees) noexcept;
		void rotateY(float fDegrees) noexcept;
		void rotateZ(float fDegrees) noexcept;
	private:
		void setRotationX(float angle) noexcept;
		void setRotationY(float angle) noexcept;
		void setRotationZ(float angle) noexcept;

		void scale(float sx, float sy, float sz) noexcept;

	public:
		void translate(const vector3& test);
		[[nodiscard]] vector3 getTranslate() const noexcept;
		void setTranslate(const vector3& b) noexcept;
		void resetTranslate() noexcept;

		void toCCWRotationMatrix(float fDegrees, float x, float y, float z);
		[[nodiscard]] matrix invertSimple(); // Simple but not robust matrix inversion. (Doesn't work properly if there is a scaling or skewing transformation.)
		[[nodiscard]] matrix invertRot(); // Invert for only a rotation, any translation is zeroed out

		[[nodiscard]] vector3 getRightVector() const noexcept;
		[[nodiscard]] vector3 getForwardVector() const noexcept;
		[[nodiscard]] vector3 getUpVector() const noexcept;
	};
#pragma warning(pop)
#pragma pack(pop)
}
