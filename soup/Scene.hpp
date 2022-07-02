#pragma once

#include "fwd.hpp"

#include "PointLight.hpp"
#include "Poly.hpp"
#include "Rgb.hpp"
#include <vector>
#include "Vector3.hpp"

namespace soup
{
	class Scene
	{
	public:
		struct Tri
		{
			Poly p;
			Rgb colour;
			bool not_reflective = false;
		};

		std::vector<Tri> tris{};
		PointLight light{ { 0.0f, 0.0f, 10.0f } };
		Vector3 cam_pos = { 0.0f, 0.0f, 0.0f };
		Vector3 cam_rot = { 0.0f, 0.0f, 0.0f };
		float render_distance = 100.0f;
		uint8_t max_ray_bounces = 5;
		Rgb sky_colour{ 127, 127, 127 };

		[[nodiscard]] Matrix getCameraMatrix() const; // "Point at" / camera to world matrix
		[[nodiscard]] Matrix getLookAtMatrix() const; // "Look at" / world to camera matrix
		[[nodiscard]] Vector2 world2screen(const Vector3& pos) const;

		[[nodiscard]] bool intersect(const Ray& r, Vector3* outHitPos = nullptr, const Tri** outHitTri = nullptr) const;
		[[nodiscard]] bool hasLineOfSight(const Vector3& p1, const Vector3& p2) const;

		void renderOnto(Canvas& c, float fov);
	protected:
		[[nodiscard]] Rgb raytrace(float x, float y, float fov, float ratio) const;
		[[nodiscard]] Rgb raytrace(const Ray& r, uint8_t depth = 0) const;

	public:
		[[nodiscard]] Ray getFollowupRay(Vector3 pos, const Vector3& dir) const;
	};
}
