#pragma once

#include "fwddecl.hpp"

#include <array>

#include "matrix.hpp"
#include "vector3.hpp"

namespace soup
{
	/**
	 * From https://www.3dkingdoms.com/weekly/weekly.php?a=21
	 * 
	 * To store the box, we need to store a centerpoint, a rotation, and how far the box extends from the centerpoint in the x,y,z directions.
	 * I use a 4x4 matrix to store the rotation and centerpoint translation, and a vector for the Extents.
	 * I just chose to use a 4x4 matrix for simplicity so I could use the matrix class.
	 * An alternative to storing extents and a centerpoint is storing the minimum corner and maximum corner, so I have a set function that converts from this format.
	 */

#pragma pack(push, 1)
#pragma warning(push)
#pragma warning(disable: 26495) // uninitialised member variable
	struct box
	{
		matrix m_M;
		vector3 m_Extent;

		box() {}
		box(const matrix& M, const vector3& Extent)
		{
			set(M, Extent);
		}
		// BL = Low values corner point, BH = High values corner point
		box(const matrix& M, const vector3& BL, const vector3& BH)
		{
			set(M, BL, BH);
		}

		box(const vector3& pos, const vector3& rot, const vector3& dimensions)
			: m_M(pos, rot), m_Extent(dimensions * 0.5f)
		{

		}

		void set(const matrix& M, const vector3& Extent)
		{
			m_M = M;
			m_Extent = Extent;
		}
		void set(const matrix& M, const vector3& BL, const vector3& BH)
		{
			m_M = M;
			m_M.translate((BH + BL) * 0.5f);
			m_Extent = (BH - BL) / 2.0f;
		}

		[[nodiscard]] vector3 getSize() const noexcept
		{
			return m_Extent * 2.0f;
		}
		[[nodiscard]] vector3 getCentrePoint() const noexcept
		{
			return m_M.getTranslate();
		}
		void getInvRot(vector3* pvRot);

		bool checkRayIntersection(const ray& r, vector3* outHit = nullptr);

		bool isPointInBox(const vector3& P);
		bool isBoxInBox(box& BBox);
		bool isSphereInBox(const vector3& P, float fRadius);
		bool boxOutsidePlane(const vector3& Norm, const vector3& P);

		[[nodiscard]] box_corners toCorners() const noexcept;
		[[nodiscard]] std::array<poly, 12> toPolys() const noexcept;
	};
#pragma warning(pop)
#pragma pack(pop)
}
