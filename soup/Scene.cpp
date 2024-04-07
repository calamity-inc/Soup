#include "Scene.hpp"

#include "Canvas.hpp"
#include "Matrix.hpp"
#include "Ray.hpp"
#include "Vector2.hpp"

NAMESPACE_SOUP
{
	Matrix Scene::getCameraMatrix() const
	{
		Matrix m;
		m.setPosRotXYZ(cam_pos, cam_rot);
		return m;
	}

	Matrix Scene::getLookAtMatrix() const
	{
		Matrix cameraToWorld = getCameraMatrix();
		Matrix worldToCamera = cameraToWorld.invert();
		return worldToCamera;
	}

	Vector2 Scene::world2screen(const Vector3& pos) const
	{
		Matrix worldToCamera = getLookAtMatrix();
		Vector3 v = (worldToCamera * pos);
		return { 1.0f - ((v.x + 1.0f) * 0.5f), 1.0f - ((v.z + 1.0f) * 0.5f) };
	}

	bool Scene::intersect(const Ray& r, Vector3* outHitPos, const Tri** outHitTri) const
	{
		float best_dist = FLT_MAX;
		Vector3 best_point;
		const Tri* best_tri;
		for (const auto& t : tris)
		{
			Vector3 p;
			if (t.p.checkRayIntersection(r, p))
			{
				float dist = p.distance(r.start);
				if (dist < best_dist)
				{
					best_dist = dist;
					best_point = p;
					best_tri = &t;
				}
			}
		}
		if (best_dist == FLT_MAX)
		{
			return false;
		}
		if (outHitPos != nullptr)
		{
			*outHitPos = best_point;
		}
		if (outHitTri != nullptr)
		{
			*outHitTri = best_tri;
		}
		return true;
	}
}
