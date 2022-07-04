#include "SceneRaytracingRenderer.hpp"

#include "Ray.hpp"
#include "RenderTarget.hpp"
#include "Scene.hpp"

namespace soup
{
	bool SceneRaytracingRenderer::hasLineOfSight(const Scene& s, const Vector3& p1, const Vector3& p2) const
	{
		return !s.intersect(getFollowupRay(s, p1, p1.lookAt(p2).toDir()));
	}

	void SceneRaytracingRenderer::render(const Scene& s, RenderTarget& rt, float fov) const
	{
		const float ratio = ((float)rt.width / rt.height) * 0.5f;
		for (unsigned int x = 0; x != rt.width; ++x)
		{
			for (unsigned int y = 0; y != rt.height; ++y)
			{
				rt.drawPixel(x, y, raytrace(s, (float)x / rt.width, (float)y / rt.height, fov, ratio));
			}
		}
	}

	Rgb SceneRaytracingRenderer::raytrace(const Scene& s, float x, float y, float fov, float ratio) const
	{
		return raytrace(s, Ray::withRot(s.cam_pos, s.cam_rot + Vector3{ (y - 0.5f) * -1.0f * fov, 0.0f, (x - 0.5f) * ratio * fov }, render_distance));
	}

	Rgb SceneRaytracingRenderer::raytrace(const Scene& s, const Ray& r, uint8_t depth) const
	{
		Vector3 hitPos;
		const Scene::Tri* hitTri;
		if (s.intersect(r, &hitPos, &hitTri))
		{
			const auto normal = hitTri->p.getSurfaceNormal();

			auto brightness = s.light.getPointBrightness(hitPos, normal);

			if (hitTri->not_reflective
				&& !hasLineOfSight(s, hitPos, s.light.pos)
				)
			{
				brightness *= 0.5f;
			}

			if (!hitTri->not_reflective
				&& depth != max_ray_bounces
				)
			{
				auto res = raytrace(s, getFollowupRay(s, hitPos, normal), ++depth);
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
		return s.sky_colour;
	}

	Ray SceneRaytracingRenderer::getFollowupRay(const Scene& s, Vector3 pos, const Vector3& dir) const
	{
		pos += (dir * 0.001f);
		return Ray::withDir(pos, dir, render_distance);
	}
}
