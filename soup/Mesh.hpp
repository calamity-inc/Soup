#pragma once

#include "fwd.hpp"

#include <string>
#include <vector>

#include "Vector3.hpp"

namespace soup
{
	struct Mesh
	{
		struct Tri
		{
			size_t a;
			size_t b;
			size_t c;
		};

		std::vector<Vector3> verts{};
		std::vector<Tri> tris{};

		[[nodiscard]] static Mesh fromObj(const std::string& obj);

		[[nodiscard]] bool empty() const noexcept;
		void clear() noexcept;

		[[nodiscard]] Vector3 getCentrePoint() const;

		size_t addVertex(const Vector3& p);
		size_t addVertex(float x, float y, float z);
		void addTriangle(size_t a, size_t b, size_t c);
		void addQuad(size_t a, size_t b, size_t c, size_t d);

		[[nodiscard]] std::vector<Poly> toPolys(const Vector3& transform = {}) const;
	};
}
