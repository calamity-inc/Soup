#pragma once

#include "fwd.hpp"

#include "point_light.hpp"
#include "poly.hpp"
#include "rgb.hpp"
#include <vector>
#include "vector3.hpp"

namespace soup
{
	class scene
	{
	public:
		struct tri
		{
			poly p;
			rgb colour;
			bool not_reflective = false;
		};

		std::vector<tri> tris{};
		point_light light{ { 0.0f, 0.0f, 10.0f } };
		vector3 cam_pos = { 0.0f, 0.0f, 0.0f };
		vector3 cam_rot = { 0.0f, 0.0f, 0.0f };
		float render_distance = 100.0f;
		uint8_t max_ray_bounces = 5;
		rgb sky_colour{ 127, 127, 127 };

		[[nodiscard]] bool intersect(const ray& r, vector3* outHitPos = nullptr, const tri** outHitTri = nullptr) const;
		[[nodiscard]] bool hasLineOfSight(const vector3& p1, const vector3& p2) const;

		void renderOnto(canvas& c, float fov);
	protected:
		[[nodiscard]] rgb raytrace(float x, float y, float fov) const;
		[[nodiscard]] rgb raytrace(const ray& r, uint8_t depth = 0) const;

	public:
		[[nodiscard]] ray getFollowupRay(vector3 pos, const vector3& dir) const;
	};
}
