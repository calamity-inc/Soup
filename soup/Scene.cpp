#include "Scene.hpp"

#include "Canvas.hpp"
#include "Ray.hpp"

namespace soup
{
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

	bool Scene::hasLineOfSight(const Vector3& p1, const Vector3& p2) const
	{
		return !intersect(getFollowupRay(p1, p1.lookAt(p2).toDir()));
	}

	void Scene::renderOnto(Canvas& c, float fov)
	{
		const float ratio = ((float)c.width / c.height) * 0.5f;
		for (unsigned int x = 0; x != c.width; ++x)
		{
			for (unsigned int y = 0; y != c.height; ++y)
			{
				c.set(x, y, raytrace((float)x / c.width, (float)y / c.height, fov, ratio));
			}
		}
	}

	Rgb Scene::raytrace(float x, float y, float fov, float ratio) const
	{
		return raytrace(Ray::withRot(cam_pos, cam_rot + Vector3{ (y - 0.5f) * -1.0f * fov, 0.0f, (x - 0.5f) * ratio * fov }, render_distance));
	}

	Rgb Scene::raytrace(const Ray& r, uint8_t depth) const
	{
		Vector3 hitPos;
		const Tri* hitTri;
		if (intersect(r, &hitPos, &hitTri))
		{
			const auto normal = hitTri->p.getSurfaceNormal();

			auto brightness = light.getPointBrightness(hitPos, normal);

			if (hitTri->not_reflective
				&& !hasLineOfSight(hitPos, light.pos)
				)
			{
				brightness *= 0.5f;
			}

			if (!hitTri->not_reflective
				&& depth != max_ray_bounces
				)
			{
				auto res = raytrace(getFollowupRay(hitPos, normal), ++depth);
				return Rgb{
					(uint8_t)((float)res.r * (1.0f - brightness - ((float)hitTri->colour.r / 255.0f))),
					(uint8_t)((float)res.g * (1.0f - brightness - ((float)hitTri->colour.g / 255.0f))),
					(uint8_t)((float)res.b * (1.0f - brightness - ((float)hitTri->colour.b / 255.0f)))
				};
			}
			else
			{
				return Rgb{
					(uint8_t)(brightness * hitTri->colour.r),
					(uint8_t)(brightness * hitTri->colour.g),
					(uint8_t)(brightness * hitTri->colour.b)
				};
			}
		}
		return sky_colour;
	}

	Ray Scene::getFollowupRay(Vector3 pos, const Vector3& dir) const
	{
		pos += (dir * 0.001f);
		return Ray::withDir(pos, dir, render_distance);
	}
}
