#include "tridText.hpp"

#include "RasterFont.hpp"

namespace soup
{
    std::vector<Box> tridText::getVoxels() const
	{
		std::vector<Box> boxes{};
		Vector3 extent = (voxel_dimensions * 0.5f);
		std::swap(extent.x, extent.y);
		Matrix mat(this->mat);
		for (const auto& c : text)
		{
			const auto& g = font->get(c);
			for (uint8_t x = 0; x != g.width; ++x)
			{
				for (uint8_t y = 0; y != g.height; ++y)
				{
					if (g.getPixel(x, y))
					{
						Matrix m(mat);
						m.translate(Vector3{
							(x * voxel_dimensions.x),
							0.0f,
							-((y + g.y_offset) * voxel_dimensions.z)
						});
						boxes.emplace_back(Box(std::move(m), extent));
					}
				}
			}
			mat.translate(Vector3{
				((g.width + 1) * voxel_dimensions.x),
				0.0f,
				0.0f
			});
		}
        return boxes;
    }

	std::vector<Poly> tridText::toPolys() const
	{
		auto voxels = getVoxels();

		std::vector<Poly> polys;
		polys.reserve(voxels.size() * 12);

		for (const auto& b : voxels)
		{
			for (const auto& p : b.toPolys())
			{
				polys.emplace_back(std::move(p));
			}
		}

		return polys;
	}
}
