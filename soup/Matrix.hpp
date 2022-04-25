#pragma once

#include "fwd.hpp"

namespace soup
{
#pragma pack(push, 1)
#pragma warning(push)
#pragma warning(disable: 26495) // uninitialised member variable
	class Matrix
	{
	public:
		float mf[16];

		Matrix() noexcept;
		Matrix(const Vector3& pos, const Vector3& rot) noexcept;

		void reset() noexcept; // set to identity

		[[nodiscard]] Matrix operator* (const Matrix& InM) const noexcept;
		void operator*= (const Matrix& InM) noexcept;
		[[nodiscard]] Vector3 operator* (const Vector3& Point) const noexcept;

		void resetRotation() noexcept;
		void setRotation(const Vector3& rot) noexcept;
		void rotate(const Vector3& rot) noexcept;
		void rotateCCW(const Vector3& rot) noexcept;
		void rotateX(float fDegrees) noexcept;
		void rotateY(float fDegrees) noexcept;
		void rotateZ(float fDegrees) noexcept;
	private:
		void setRotationX(float angle) noexcept;
		void setRotationY(float angle) noexcept;
		void setRotationZ(float angle) noexcept;

		void scale(float sx, float sy, float sz) noexcept;

	public:
		void translate(const Vector3& test);
		[[nodiscard]] Vector3 getTranslate() const noexcept;
		void setTranslate(const Vector3& b) noexcept;
		void resetTranslate() noexcept;

		void toCCWRotationMatrix(float fDegrees, float x, float y, float z);

		[[nodiscard]] Matrix invert() const noexcept;
		[[nodiscard]] Matrix invertSimple() const noexcept; // Simple but not robust matrix inversion. (Doesn't work properly if there is a scaling or skewing transformation.)
		[[nodiscard]] Matrix invertRot() const noexcept; // Invert for only a rotation, any translation is zeroed out

		[[nodiscard]] Vector3 getRightVector() const noexcept;
		[[nodiscard]] Vector3 getForwardVector() const noexcept;
		[[nodiscard]] Vector3 getUpVector() const noexcept;
	};
#pragma warning(pop)
#pragma pack(pop)
}
