#pragma once

#include "SceneRenderer.hpp"

namespace soup
{
	class SceneRaytracingRenderer : public SceneRenderer
	{
	public:
		float render_distance = 100.0f;
		uint8_t max_ray_bounces = 5;

		[[nodiscard]] bool hasLineOfSight(const Scene& s, const Vector3& p1, const Vector3& p2) const;

		void render(const Scene& s, RenderTarget& rt, float fov) const final;
	protected:
		[[nodiscard]] Rgb raytrace(const Scene& s, float x, float y, float fov, float ratio) const;
		[[nodiscard]] Rgb raytrace(const Scene& s, const Ray& r, uint8_t depth = 0) const;

	public:
		[[nodiscard]] Ray getFollowupRay(const Scene& s, Vector3 pos, const Vector3& dir) const;
	};
}
