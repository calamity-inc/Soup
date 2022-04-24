#pragma once

#include "fwd.hpp"

#include "Sphere.hpp"
#include <vector>

namespace soup
{
	struct UvSphere : public Sphere
	{
		[[nodiscard]] Mesh toMesh(unsigned int quality) const;
		[[nodiscard]] Mesh toMesh(unsigned int slices, unsigned int stacks) const;
		[[nodiscard]] std::vector<Poly> toPolys(unsigned int quality) const;
	};
}
