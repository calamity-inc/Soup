#pragma once

#include "fwd.hpp"

#include "sphere.hpp"
#include <vector>

namespace soup
{
	struct uv_sphere : public sphere
	{
		[[nodiscard]] mesh toMesh(unsigned int quality) const;
		[[nodiscard]] mesh toMesh(unsigned int slices, unsigned int stacks) const;
		[[nodiscard]] std::vector<poly> toPolys(unsigned int quality) const;
	};
}
