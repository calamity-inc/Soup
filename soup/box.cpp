#include "box.hpp"

#include "box_corners.hpp"
#include "poly.hpp"
#include "ray.hpp"

namespace soup
{
	//
	// Returns a 3x3 rotation matrix as vectors
	//
	void box::getInvRot(vector3* pvRot)
	{
		pvRot[0] = vector3(m.mf[0], m.mf[1], m.mf[2]);
		pvRot[1] = vector3(m.mf[4], m.mf[5], m.mf[6]);
		pvRot[2] = vector3(m.mf[8], m.mf[9], m.mf[10]);
	}

	bool box::checkRayIntersection(const ray& r, vector3* outHit)
	{
		// Put line in box space
		matrix MInv = m.invertSimple();
		vector3 LB1 = MInv * r.start;
		vector3 LB2 = MInv * r.end;

		// Get line midpoint and extent
		vector3 LMid = (LB1 + LB2) * 0.5f;
		vector3 L = (LB1 - LMid);
		vector3 LExt = vector3(fabsf(L.x), fabsf(L.y), fabsf(L.z));

		// Use Separating Axis Test
		// Separation vector from box center to line center is LMid, since the line is in box space
		if (fabsf(LMid.x) > extent.x + LExt.x) return false;
		if (fabsf(LMid.y) > extent.y + LExt.y) return false;
		if (fabsf(LMid.z) > extent.z + LExt.z) return false;
		// Crossproducts of line and each axis
		if (fabsf(LMid.y * L.z - LMid.z * L.y) > (extent.y * LExt.z + extent.z * LExt.y)) return false;
		if (fabsf(LMid.x * L.z - LMid.z * L.x) > (extent.x * LExt.z + extent.z * LExt.x)) return false;
		if (fabsf(LMid.x * L.y - LMid.y * L.x) > (extent.x * LExt.y + extent.y * LExt.x)) return false;
		// No separating axis, the line intersects
		if (outHit != nullptr)
		{
			float best_dist = FLT_MAX;
			vector3 best_point;
			for (const auto& t : toPolys())
			{
				vector3 p;
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
			*outHit = best_point;
		}
		return true;
	}

	//
	// Check if a point is in this bounding box
	//
	bool box::isPointInBox(const vector3& InP)
	{
		// Rotate the point into the box's coordinates
		vector3 P = m.invertSimple() * InP;

		// Now just use an axis-aligned check
		if (fabsf(P.x) < extent.x && fabsf(P.y) < extent.y && fabsf(P.z) < extent.z)
			return true;

		return false;
	}

	//
	// Check if a sphere overlaps any part of this bounding box
	//
	bool box::isSphereInBox(const vector3& InP, float fRadius)
	{
		float fDist;
		float fDistSq = 0;
		vector3 P = m.invertSimple() * InP;

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
	bool box::boxOutsidePlane(const vector3& InNorm, const vector3& InP)
	{
		// Plane Normal in Box Space
		vector3 Norm = InNorm.rotateByMatrix(m.invertSimple().mf); // RotByMatrix only uses rotation portion of matrix
		Norm = vector3(fabsf(Norm.x), fabsf(Norm.y), fabsf(Norm.z));

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
	bool box::isBoxInBox(box& BBox)
	{
		vector3 SizeA = extent;
		vector3 SizeB = BBox.extent;
		vector3 RotA[3], RotB[3];
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
		vector3 vSepWS = BBox.getCentrePoint() - getCentrePoint();
		// Rotated into Box A's coordinates
		vector3 vSepA(vSepWS.dot(RotA[0]), vSepWS.dot(RotA[1]), vSepWS.dot(RotA[2]));

		// Test if any of A's basis vectors separate the box
		for (i = 0; i < 3; i++)
		{
			ExtentA = SizeA[i];
			ExtentB = SizeB.dot(vector3(AR[i][0], AR[i][1], AR[i][2]));
			Separation = fabsf(vSepA[i]);

			if (Separation > ExtentA + ExtentB) return false;
		}

		// Test if any of B's basis vectors separate the box
		for (k = 0; k < 3; k++)
		{
			ExtentA = SizeA.dot(vector3(AR[0][k], AR[1][k], AR[2][k]));
			ExtentB = SizeB[k];
			Separation = fabsf(vSepA.dot(vector3(R[0][k], R[1][k], R[2][k])));

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

	box_corners box::toCorners() const noexcept
	{
		return box_corners(
			getCentrePoint(),
			extent,
			vector3(m.mf[0], m.mf[1], m.mf[2]),
			vector3(m.mf[4], m.mf[5], m.mf[6]),
			vector3(m.mf[8], m.mf[9], m.mf[10])
		);
	}

	std::array<poly, 12> box::toPolys() const noexcept
	{
		return toCorners().toPolys();
	}
}
