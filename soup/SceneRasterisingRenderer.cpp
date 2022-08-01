#include "SceneRasterisingRenderer.hpp"

#include <deque>

#include "Matrix.hpp"
#include "RenderTarget.hpp"
#include "Scene.hpp"
#include "Vector2.hpp"

namespace soup
{
	static void translatePos(Vector3& v)
	{
		v.y *= -1.0f;
		v.z *= -1.0f;
		std::swap(v.y, v.z);
	}

	static void translateRot(Vector3& v)
	{
		v.z += 180.0f;
		std::swap(v.y, v.z);
	}

	static Vector3 Vector_IntersectPlane(const Vector3& plane_p, Vector3& plane_n, const Vector3& lineStart, const Vector3& lineEnd)
	{
		plane_n.normalise();
		float plane_d = -plane_n.dot(plane_p);
		float ad = lineStart.dot(plane_n);
		float bd = lineEnd.dot(plane_n);
		float t = (-plane_d - ad) / (bd - ad);
		Vector3 lineStartToEnd = (lineEnd - lineStart);
		Vector3 lineToIntersect = (lineStartToEnd * t);
		return lineStart + lineToIntersect;
	}

	// Return signed shortest distance from point to plane, plane normal must be normalised
	static float Vector3_DistanceToPlane(const Vector3& plane_p, const Vector3& plane_n, const Vector3& p)
	{
		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.dot(plane_p));
	}

	static int Triangle_ClipAgainstPlane(Vector3 plane_p, Vector3 plane_n, Poly& in_tri, Poly& out_tri1, Poly& out_tri2)
	{
		// Make sure plane normal is indeed normal
		plane_n.normalise();

		// Create two temporary storage arrays to classify points either side of plane
		// If distance sign is positive, point lies on "inside" of plane
		Vector3* inside_points[3];  int nInsidePointCount = 0;
		Vector3* outside_points[3]; int nOutsidePointCount = 0;

		// Get signed distance of each point in triangle to plane
		float d0 = Vector3_DistanceToPlane(plane_p, plane_n, in_tri.a);
		float d1 = Vector3_DistanceToPlane(plane_p, plane_n, in_tri.b);
		float d2 = Vector3_DistanceToPlane(plane_p, plane_n, in_tri.c);

		if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.a; }
		else { outside_points[nOutsidePointCount++] = &in_tri.a; }
		if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.b; }
		else { outside_points[nOutsidePointCount++] = &in_tri.b; }
		if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.c; }
		else { outside_points[nOutsidePointCount++] = &in_tri.c; }

		// Now classify triangle points, and break the input triangle into 
		// smaller output triangles if required. There are four possible
		// outcomes...

		if (nInsidePointCount == 0)
		{
			// All points lie on the outside of plane, so clip whole triangle
			// It ceases to exist

			return 0; // No returned triangles are valid
		}

		if (nInsidePointCount == 3)
		{
			// All points lie on the inside of plane, so do nothing
			// and allow the triangle to simply pass through
			out_tri1 = in_tri;

			return 1; // Just the one returned original triangle is valid
		}

		if (nInsidePointCount == 1 && nOutsidePointCount == 2)
		{
			// Triangle should be clipped. As two points lie outside
			// the plane, the triangle simply becomes a smaller triangle

			// Copy appearance info to new triangle
			//out_tri1.col = in_tri.col;
			//out_tri1.sym = in_tri.sym;

			// The inside point is valid, so keep that...
			out_tri1.a = *inside_points[0];

			// but the two new points are at the locations where the 
			// original sides of the triangle (lines) intersect with the plane
			out_tri1.b = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
			out_tri1.c = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

			return 1; // Return the newly formed single triangle
		}

		if (nInsidePointCount == 2 && nOutsidePointCount == 1)
		{
			// Triangle should be clipped. As two points lie inside the plane,
			// the clipped triangle becomes a "quad". Fortunately, we can
			// represent a quad with two new triangles

			// Copy appearance info to new triangles
			//out_tri1.col = in_tri.col;
			//out_tri1.sym = in_tri.sym;

			//out_tri2.col = in_tri.col;
			//out_tri2.sym = in_tri.sym;

			// The first triangle consists of the two inside points and a new
			// point determined by the location where one side of the triangle
			// intersects with the plane
			out_tri1.a = *inside_points[0];
			out_tri1.b = *inside_points[1];
			out_tri1.c = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

			// The second triangle is composed of one of he inside points, a
			// new point determined by the intersection of the other side of the 
			// triangle and the plane, and the newly created point above
			out_tri2.a = *inside_points[1];
			out_tri2.b = out_tri1.c;
			out_tri2.c = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

			return 2; // Return two newly formed triangles which form a quad
		}

		return 0; // No returned triangles are valid
	}

	static void screenClipAndDraw(const Scene& s, RenderTarget& rt, Poly&& p, const Rgb& colour)
	{
		Poly clipped[2];
		std::deque<Poly> listTriangles;

		// Add initial triangle
		listTriangles.push_back(std::move(p));
		int nNewTriangles = 1;

		for (int p = 0; p != 4; ++p)
		{
			int nTrisToAdd = 0;
			while (nNewTriangles != 0)
			{
				// Take triangle from front of queue
				Poly test = listTriangles.front();
				listTriangles.pop_front();
				--nNewTriangles;

				// Clip it against a plane. We only need to test each 
				// subsequent plane, against subsequent new triangles
				// as all triangles after a plane clip are guaranteed
				// to lie on the inside of the plane. I like how this
				// comment is almost completely and utterly justified
				switch (p)
				{
				case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)rt.height - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)rt.width - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				}

				// Clipping may yield a variable number of triangles, so
				// add these new ones to the back of the queue for subsequent
				// clipping against next planes
				for (int w = 0; w != nTrisToAdd; ++w)
				{
					listTriangles.push_back(clipped[w]);
				}
			}
			nNewTriangles = listTriangles.size();
		}

		// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
		for (auto& p : listTriangles)
		{
			rt.drawTriangle(Vector2{ p.a.x, p.a.y }, Vector2{ p.b.x, p.b.y }, Vector2{ p.c.x, p.c.y }, colour);
		}
	}

	void SceneRasterisingRenderer::render(const Scene& s, RenderTarget& rt, float fov) const
	{
		rt.fill(s.sky_colour);

		Vector3 cam_pos_fixed = s.cam_pos;
		translatePos(cam_pos_fixed);

		Vector3 cam_rot_fixed = s.cam_rot;
		translateRot(cam_rot_fixed);

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
				translatePos(p.a);
				translatePos(p.b);
				translatePos(p.c);

				// World -> View
				p.a = look_at * p.a;
				p.b = look_at * p.b;
				p.c = look_at * p.c;

				Poly clipped[2];
				int nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, z_near }, { 0.0f, 0.0f, 1.0f }, p, clipped[0], clipped[1]);
				for (int n = 0; n != nClippedTriangles; ++n)
				{
					p = clipped[n];

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

					// TODO: Sort triangles before drawing as a poor person's depth buffer (https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_olcEngine3D_Part3.cpp#L634)

					float l = s.light.getPointBrightness(p.a, normal);
					Rgb colour = t.colour;
					colour.r *= l;
					colour.g *= l;
					colour.b *= l;
					screenClipAndDraw(s, rt, std::move(p), colour);
				}
			}
		}
	}
}
