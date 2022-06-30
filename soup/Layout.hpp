#pragma once

#include <string>
#include <vector>

#include "fwd.hpp"

namespace soup
{
	struct Layout
	{
		struct Element
		{
			std::string text;
			uint8_t font_size;
		};

		struct Block
		{
			uint8_t margin;
			std::vector<Element> elms{};
		};

		std::vector<Block> blocks{};

		void draw(RenderTarget& rt);
	};
}
