#include "Mesh.hpp"

#include "Poly.hpp"

namespace soup
{
	Vector3 Mesh::getCentrePoint() const
	{
		Vector3 centre{};
		for (const auto& p : verts)
		{
			centre += p;
		}
		centre /= (float)verts.size();
		return centre;
	}

	size_t Mesh::addVertex(const Vector3& p)
	{
		auto ret = verts.size();
		verts.emplace_back(p);
		return ret;
	}

	size_t Mesh::addVertex(float x, float y, float z)
	{
		return addVertex(Vector3{ x, y, z });
	}

	void Mesh::addTriangle(size_t a, size_t b, size_t c)
	{
		tris.emplace_back(Tri{ a, b, c });
	}

	void Mesh::addQuad(size_t a, size_t b, size_t c, size_t d)
	{
		addTriangle(c, b, a);
		addTriangle(d, c, a);
	}

	std::vector<Poly> Mesh::toPolys(const Vector3& transform) const
	{
		std::vector<Poly> polys{};
		for (const auto& tri : tris)
		{
			polys.emplace_back(Poly{
				transform + verts.at(tri.a),
				transform + verts.at(tri.b),
				transform + verts.at(tri.c)
				});
		}
		return polys;
	}
}
