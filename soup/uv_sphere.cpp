#include "uv_sphere.hpp"

#include "mesh.hpp"
#include "poly.hpp"

namespace soup
{
	mesh uv_sphere::toMesh(unsigned int quality) const
	{
		return toMesh(quality, quality);
	}

	mesh uv_sphere::toMesh(unsigned int slices, unsigned int stacks) const
	{
		mesh m;

		// add top vertex
		auto v0 = m.addVertex(0.0f, 0.0f, radius);

		// generate vertices per stack / slice
		for (size_t i = 0; i != stacks - 1; ++i)
		{
			auto phi = (float)M_PI * float(i + 1) / float(stacks);
			for (size_t j = 0; j != slices; ++j)
			{
				auto theta = 2.0f * (float)M_PI * float(j) / float(slices);
				vector3 vert{
					sinf(phi) * cosf(theta),
					sinf(phi) * sinf(theta),
					cosf(phi)
				};
				vert *= radius;
				m.addVertex(vert);
			}
		}

		// add bottom vertex
		auto v1 = m.addVertex(0.0f, 0.0f, -radius);

		// add top / bottom triangles
		for (size_t i = 0; i != slices; ++i)
		{
			auto i0 = i + 1;
			auto i1 = (i + 1) % slices + 1;
			m.addTriangle(i0, i1, v0);
			i0 = i + slices * (stacks - 2) + 1;
			i1 = (i + 1) % slices + slices * (stacks - 2) + 1;
			m.addTriangle(i1, i0, v1);
		}

		// add quads per stack / slice
		for (size_t j = 0; j != stacks - 2; ++j)
		{
			auto j0 = j * slices + 1;
			auto j1 = (j + 1) * slices + 1;
			for (size_t i = 0; i != slices; ++i)
			{
				auto i0 = j0 + i;
				auto i1 = j0 + (i + 1) % slices;
				auto i2 = j1 + (i + 1) % slices;
				auto i3 = j1 + i;
				m.addQuad(i0, i1, i2, i3);
			}
		}

		return m;
	}

	std::vector<poly> uv_sphere::toPolys(unsigned int quality) const
	{
		return toMesh(quality).toPolys(origin);
	}
}
