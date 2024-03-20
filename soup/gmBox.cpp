#include "gmBox.hpp"

#include "gmBoxCorners.hpp"
#include "Poly.hpp"
#include "Ray.hpp"

namespace soup
{
	//
	// Returns a 3x3 rotation matrix as vectors
	//
	void gmBox::getInvRot(Vector3* pvRot)
	{
		pvRot[0] = Vector3(m.mf[0], m.mf[1], m.mf[2]);
		pvRot[1] = Vector3(m.mf[4], m.mf[5], m.mf[6]);
		pvRot[2] = Vector3(m.mf[8], m.mf[9], m.mf[10]);
	}

	bool gmBox::checkRayIntersection(const Ray& r, Vector3* outHit)
	{
		//// Put line in box space
		//matrix MInv = m.invertSimple();
		//vector3 LB1 = MInv * r.start;
		//vector3 LB2 = MInv * r.end;
		//
		//// Get line midpoint and extent
		//vector3 LMid = (LB1 + LB2) * 0.5f;
		//vector3 L = (LB1 - LMid);
		//vector3 LExt = vector3(fabsf(L.x), fabsf(L.y), fabsf(L.z));
		//
		//// Use Separating Axis Test
		//// Separation vector from box center to line center is LMid, since the line is in box space
		//if (fabsf(LMid.x) > extent.x + LExt.x) return false;
		//if (fabsf(LMid.y) > extent.y + LExt.y) return false;
		//if (fabsf(LMid.z) > extent.z + LExt.z) return false;
		//// Crossproducts of line and each axis
		//if (fabsf(LMid.y * L.z - LMid.z * L.y) > (extent.y * LExt.z + extent.z * LExt.y)) return false;
		//if (fabsf(LMid.x * L.z - LMid.z * L.x) > (extent.x * LExt.z + extent.z * LExt.x)) return false;
		//if (fabsf(LMid.x * L.y - LMid.y * L.x) > (extent.x * LExt.y + extent.y * LExt.x)) return false;
		//// No separating axis, the line intersects
		//if (outHit != nullptr)
		//{
		//	float best_dist = FLT_MAX;
		//	vector3 best_point;
		//	for (const auto& t : toPolys())
		//	{
		//		vector3 p;
		//		if (t.checkRayIntersection(r, p))
		//		{
		//			float dist = p.distance(r.start);
		//			if (dist < best_dist)
		//			{
		//				best_dist = dist;
		//				best_point = p;
		//			}
		//		}
		//	}
		//	*outHit = best_point;
		//}
		//return true;

		// The other implementation has precision issues, even with trivial rotations. Might be worth revisiting it, if performance was critical.
		float best_dist = FLT_MAX;
		Vector3 best_point;
		for (const auto& t : toPolys())
		{
			Vector3 p;
			if (t.checkRayIntersection(r, p))
			{
				float dist = p.distance(r.start);
				if (dist < best_dist)
				{
					best_dist = dist;
					best_point = p;
				}
			}
		}
		if (best_dist == FLT_MAX)
		{
			return false;
		}
		if (outHit != nullptr)
		{
			*outHit = best_point;
		}
		return true;
	}

	//
	// Check if a point is in this bounding box
	//
	bool gmBox::isPointInBox(const Vector3& InP)
	{
		// Rotate the point into the box's coordinates
		Vector3 P = m.invertSimple() * InP;

		// Now just use an axis-aligned check
		if (fabsf(P.x) < extent.x && fabsf(P.y) < extent.y && fabsf(P.z) < extent.z)
			return true;

		return false;
	}

	//
	// Check if a sphere overlaps any part of this bounding box
	//
	bool gmBox::isSphereInBox(const Vector3& InP, float fRadius)
	{
		float fDist;
		float fDistSq = 0;
		Vector3 P = m.invertSimple() * InP;

		// Add distance squared from sphere centerpoint to box for each axis
		for (int i = 0; i < 3; i++)
		{
			if (fabsf(P[i]) > extent[i])
			{
				fDist = fabsf(P[i]) - extent[i];
				fDistSq += fDist * fDist;
			}
		}
		return (fDistSq <= fRadius * fRadius);
	}

	//
	// Check if the bounding box is completely behind a plane( defined by a normal and a point )
	//
	bool gmBox::boxOutsidePlane(const Vector3& InNorm, const Vector3& InP)
	{
		// Plane Normal in Box Space
		Vector3 Norm = InNorm.rotateByMatrix(m.invertSimple().mf); // RotByMatrix only uses rotation portion of matrix
		Norm = Vector3(fabsf(Norm.x), fabsf(Norm.y), fabsf(Norm.z));

		float Extent = Norm.dot(extent); // Box Extent along the plane normal
		float Distance = InNorm.dot(getCentrePoint() - InP); // Distance from Box Center to the Plane

		// If Box Centerpoint is behind the plane further than its extent, the Box is outside the plane
		if (Distance < -Extent) return true;
		return false;
	}

	//
	// Check if any part of a box is inside any part of another box
	// Uses the separating axis test.
	//
	bool gmBox::isBoxInBox(gmBox& BBox)
	{
		Vector3 SizeA = extent;
		Vector3 SizeB = BBox.extent;
		Vector3 RotA[3], RotB[3];
		getInvRot(RotA);
		BBox.getInvRot(RotB);

		float R[3][3];  // Rotation from B to A
		float AR[3][3]; // absolute values of R matrix, to use with box extents
		float ExtentA, ExtentB, Separation;
		int i, k;

		// Calculate B to A rotation matrix
		for (i = 0; i < 3; i++)
			for (k = 0; k < 3; k++)
			{
				R[i][k] = RotA[i].dot(RotB[k]);
				AR[i][k] = fabsf(R[i][k]);
			}

		// Vector separating the centers of Box B and of Box A
		Vector3 vSepWS = BBox.getCentrePoint() - getCentrePoint();
		// Rotated into Box A's coordinates
		Vector3 vSepA(vSepWS.dot(RotA[0]), vSepWS.dot(RotA[1]), vSepWS.dot(RotA[2]));

		// Test if any of A's basis vectors separate the box
		for (i = 0; i < 3; i++)
		{
			ExtentA = SizeA[i];
			ExtentB = SizeB.dot(Vector3(AR[i][0], AR[i][1], AR[i][2]));
			Separation = fabsf(vSepA[i]);

			if (Separation > ExtentA + ExtentB) return false;
		}

		// Test if any of B's basis vectors separate the box
		for (k = 0; k < 3; k++)
		{
			ExtentA = SizeA.dot(Vector3(AR[0][k], AR[1][k], AR[2][k]));
			ExtentB = SizeB[k];
			Separation = fabsf(vSepA.dot(Vector3(R[0][k], R[1][k], R[2][k])));

			if (Separation > ExtentA + ExtentB) return false;
		}

		// Now test Cross Products of each basis vector combination ( A[i], B[k] )
		for (i = 0; i < 3; i++)
			for (k = 0; k < 3; k++)
			{
				int i1 = (i + 1) % 3, i2 = (i + 2) % 3;
				int k1 = (k + 1) % 3, k2 = (k + 2) % 3;
				ExtentA = SizeA[i1] * AR[i2][k] + SizeA[i2] * AR[i1][k];
				ExtentB = SizeB[k1] * AR[i][k2] + SizeB[k2] * AR[i][k1];
				Separation = fabsf(vSepA[i2] * R[i1][k] - vSepA[i1] * R[i2][k]);
				if (Separation > ExtentA + ExtentB) return false;
			}

		// No separating axis found, the boxes overlap
		return true;
	}

	gmBoxCorners gmBox::toCorners() const noexcept
	{
		return gmBoxCorners(
			getCentrePoint(),
			extent,
			m.getRightVector(),
			m.getForwardVector(),
			m.getUpVector()
		);
	}

	std::array<Poly, 12> gmBox::toPolys() const noexcept
	{
		return toCorners().toPolys();
	}
}
