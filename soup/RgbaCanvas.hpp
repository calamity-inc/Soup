#pragma once

#include <vector>

#include "fwd.hpp"
#include "Rgba.hpp"

NAMESPACE_SOUP
{
	struct RgbaCanvas
	{
		unsigned int width = 0;
		unsigned int height = 0;
		std::vector<Rgba> pixels{};

		void fill(const Rgba colour);
		void set(unsigned int x, unsigned int y, Rgba colour) noexcept;
		[[nodiscard]] Rgba get(unsigned int x, unsigned int y) const;
		[[nodiscard]] const Rgba& ref(unsigned int x, unsigned int y) const;

		void subtractGreen();

		[[nodiscard]] static RgbaCanvas fromCanvas(const Canvas& solid, uint8_t alpha = 0xFF);

		bool toBmp(Writer& w) const;
	};
}
