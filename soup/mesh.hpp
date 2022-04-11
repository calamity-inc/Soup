#pragma once

#include "fwd.hpp"

#include <vector>
#include "vector3.hpp"

namespace soup
{
	struct mesh
	{
		struct tri
		{
			size_t a;
			size_t b;
			size_t c;
		};

		std::vector<vector3> verts{};
		std::vector<tri> tris{};

		[[nodiscard]] vector3 getCentrePoint() const;

		size_t addVertex(const vector3& p);
		size_t addVertex(float x, float y, float z);
		void addTriangle(size_t a, size_t b, size_t c);
		void addQuad(size_t a, size_t b, size_t c, size_t d);

		[[nodiscard]] std::vector<poly> toPolys(const vector3& transform = {}) const;
	};
}
