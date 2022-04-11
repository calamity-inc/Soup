#include "mesh.hpp"

#include "poly.hpp"

namespace soup
{
	vector3 mesh::getCentrePoint() const
	{
		vector3 centre{};
		for (const auto& p : verts)
		{
			centre += p;
		}
		centre /= (float)verts.size();
		return centre;
	}

	size_t mesh::addVertex(const vector3& p)
	{
		auto ret = verts.size();
		verts.emplace_back(p);
		return ret;
	}

	size_t mesh::addVertex(float x, float y, float z)
	{
		return addVertex(vector3{ x, y, z });
	}

	void mesh::addTriangle(size_t a, size_t b, size_t c)
	{
		tris.emplace_back(tri{ a, b, c });
	}

	void mesh::addQuad(size_t a, size_t b, size_t c, size_t d)
	{
		addTriangle(c, b, a);
		addTriangle(d, c, a);
	}

	std::vector<poly> mesh::toPolys(const vector3& transform) const
	{
		std::vector<poly> polys{};
		for (const auto& tri : tris)
		{
			polys.emplace_back(poly{
				transform + verts.at(tri.a),
				transform + verts.at(tri.b),
				transform + verts.at(tri.c)
				});
		}
		return polys;
	}
}
