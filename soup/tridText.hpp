#pragma once

#include "fwd.hpp"

#include <string>
#include <vector>

#include "Box.hpp"
#include "Matrix.hpp"
#include "Poly.hpp"

namespace soup
{
	// "trid" is my best attempt at saying "3D" without starting with a 3
	struct tridText
	{
		Matrix mat;
		// Translate determines the origin of the text, which will be the centre of the top left voxel.
		// Rotation determines the direction in which the text goes. So a viewer would need to be at a 90 degree offset from this.

		std::u32string text;
		const RasterFont* font;
		Vector3 voxel_dimensions{ 1.0f, 1.0f, 1.0f };

		[[nodiscard]] std::vector<Box> getVoxels() const;
		[[nodiscard]] std::vector<Poly> toPolys() const;
	};
}
