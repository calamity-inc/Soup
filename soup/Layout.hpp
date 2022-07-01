#pragma once

#include "base.hpp"
#include "fwd.hpp"

#include <string>
#include <vector>

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

#if SOUP_WINDOWS
		[[nodiscard]] Window createWindow(const std::string& title);
#endif
	};
}
