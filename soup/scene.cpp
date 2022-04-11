#include "scene.hpp"

#include "canvas.hpp"
#include "ray.hpp"

namespace soup
{
	bool scene::intersect(const ray& r, vector3* outHitPos, const tri** outHitTri) const
	{
		float best_dist = FLT_MAX;
		vector3 best_point;
		const tri* best_tri;
		for (const auto& t : tris)
		{
			vector3 p;
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

	bool scene::hasLineOfSight(const vector3& p1, const vector3& p2) const
	{
		return !intersect(getFollowupRay(p1, p1.lookAt(p2).toDir()));
	}

	void scene::renderOnto(canvas& c, float fov)
	{
		for (unsigned int x = 0; x != c.width; ++x)
		{
			for (unsigned int y = 0; y != c.height; ++y)
			{
				c.set(x, y, raytrace((float)x / c.width, (float)y / c.height, fov));
			}
		}
	}

	rgb scene::raytrace(float x, float y, float fov) const
	{
		return raytrace(ray::withRot(cam_pos, cam_rot + vector3{ (y - 0.5f) * -1.0f * fov, 0.0f, (x - 0.5f) * fov }, render_distance));
	}

	rgb scene::raytrace(const ray& r, uint8_t depth) const
	{
		vector3 hitPos;
		const tri* hitTri;
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
				&& depth != 3
				)
			{
				auto res = raytrace(getFollowupRay(hitPos, normal), ++depth);
				return rgb{
					(uint8_t)((float)res.r * (1.0f - brightness - ((float)hitTri->colour.r / 255.0f))),
					(uint8_t)((float)res.g * (1.0f - brightness - ((float)hitTri->colour.g / 255.0f))),
					(uint8_t)((float)res.b * (1.0f - brightness - ((float)hitTri->colour.b / 255.0f)))
				};
			}
			else
			{
				return rgb{
					(uint8_t)(brightness * hitTri->colour.r),
					(uint8_t)(brightness * hitTri->colour.g),
					(uint8_t)(brightness * hitTri->colour.b)
				};
			}
		}
		return sky_colour;
	}

	ray scene::getFollowupRay(vector3 pos, const vector3& dir) const
	{
		pos += (dir * 0.001f);
		return ray::withDir(pos, dir, render_distance);
	}
}
