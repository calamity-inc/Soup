#pragma once

#include "fwd.hpp"

#include <array>

#include "Matrix.hpp"
#include "Vector3.hpp"

NAMESPACE_SOUP
{
	/**
	 * From https://www.3dkingdoms.com/weekly/weekly.php?a=21
	 *
	 * To store the box, we need to store a centerpoint, a rotation, and how far the box extends from the centerpoint in the x,y,z directions.
	 * I use a 4x4 matrix to store the rotation and centerpoint translation, and a vector for the Extents.
	 * I just chose to use a 4x4 matrix for simplicity so I could use the matrix class.
	 * An alternative to storing extents and a centerpoint is storing the minimum corner and maximum corner, so I have a set function that converts from this format.
	 * 
	 * (I've reached out on 1 Feb 2022 asking about the licence/legal terms, but did not receive a response.)
	 */

#pragma pack(push, 1)
#pragma warning(push)
#pragma warning(disable: 26495) // uninitialised member variable
	struct gmBox
	{
		Matrix m;
		Vector3 extent;

		gmBox() = default;

		gmBox(const Matrix& m, const Vector3& extent)
			: m(m), extent(extent)
		{
		}

		// BL = Low values corner point, BH = High values corner point
		gmBox(const Matrix& M, const Vector3& BL, const Vector3& BH)
		{
			set(M, BL, BH);
		}

		gmBox(const Vector3& pos, const Vector3& rot, const Vector3& dimensions)
			: m(pos, rot), extent(dimensions * 0.5f)
		{
		}

		void set(const Matrix& m, const Vector3& extent)
		{
			this->m = m;
			this->extent = extent;
		}

		void set(const Matrix& m, const Vector3& BL, const Vector3& BH)
		{
			this->m = m;
			this->m.translate((BH + BL) * 0.5f);
			extent = (BH - BL) / 2.0f;
		}

		[[nodiscard]] Vector3 getSize() const noexcept
		{
			return extent * 2.0f;
		}
		[[nodiscard]] Vector3 getCentrePoint() const noexcept
		{
			return m.getTranslate();
		}
		void getInvRot(Vector3* pvRot);

		bool checkRayIntersection(const Ray& r, Vector3* outHit = nullptr);

		bool isPointInBox(const Vector3& P);
		bool isBoxInBox(gmBox& BBox);
		bool isSphereInBox(const Vector3& P, float fRadius);
		bool boxOutsidePlane(const Vector3& Norm, const Vector3& P);

		[[nodiscard]] gmBoxCorners toCorners() const noexcept;
		[[nodiscard]] std::array<Poly, 12> toPolys() const noexcept;
	};
#pragma warning(pop)
#pragma pack(pop)
}
