#include "SceneRasterisingRenderer.hpp"

#include "Matrix.hpp"
#include "RenderTarget.hpp"
#include "Scene.hpp"
#include "Vector2.hpp"

namespace soup
{
	static void translateV3(Vector3& v)
	{
		v.y *= -1.0f;
		std::swap(v.y, v.z);
	}

	void SceneRasterisingRenderer::render(const Scene& s, RenderTarget& rt, float fov) const
	{
		rt.fill(s.sky_colour);

		Vector3 cam_pos_fixed = s.cam_pos;
		translateV3(cam_pos_fixed);

		Vector3 cam_rot_fixed = s.cam_rot;
		translateV3(cam_rot_fixed);

		Matrix cam; // s.getCameraMatrix
		cam.setPosRotXYZ(cam_pos_fixed, cam_rot_fixed);

		auto look_at = cam.invert();
		auto proj_mat = Matrix::projection((float)rt.height / rt.width, fov, z_near, z_far);

		std::vector<float> depth_buffer;
		depth_buffer.resize(rt.width * rt.height);

		for (const auto& t : s.tris)
		{
			auto normal = t.p.getSurfaceNormal();

			// backface culling
			auto p_to_cam = (t.p.a - cam.getTranslate());
			if (normal.dot(p_to_cam) < 0.0f)
			{
				Poly p = t.p;
				translateV3(p.a);
				translateV3(p.b);
				translateV3(p.c);

				// World -> View
				p.a = look_at * p.a;
				p.b = look_at * p.b;
				p.c = look_at * p.c;

				// View -> Projection
				p.a = proj_mat * p.a;
				p.b = proj_mat * p.b;
				p.c = proj_mat * p.c;

				// Projection -> Screen
				p.a.x += 1.0f; p.a.y += 1.0f;
				p.b.x += 1.0f; p.b.y += 1.0f;
				p.c.x += 1.0f; p.c.y += 1.0f;

				p.a.x *= 0.5f * rt.width;
				p.a.y *= 0.5f * rt.height;
				p.b.x *= 0.5f * rt.width;
				p.b.y *= 0.5f * rt.height;
				p.c.x *= 0.5f * rt.width;
				p.c.y *= 0.5f * rt.height;

				// TODO: Add depth buffer
				// TODO: Steal clipping from https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_olcEngine3D_Part3.cpp

				float l = s.light.getPointBrightness(p.a, normal);
				Rgb colour = t.colour;
				colour.r *= l;
				colour.g *= l;
				colour.b *= l;
				rt.drawTriangle(Vector2{ p.a.x, p.a.y }, Vector2{ p.b.x, p.b.y }, Vector2{ p.c.x, p.c.y }, colour);
			}
		}
	}
}
