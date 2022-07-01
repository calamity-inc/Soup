#pragma once

#include "base.hpp"
#include "fwd.hpp"

#include <string>
#include <vector>

#include "RasterFont.hpp"

namespace soup
{
	struct Layout
	{
		struct Element
		{
			std::string text;
			uint8_t font_size;
			void(*on_click)(Element&, Layout&);
		};

		struct Block
		{
			uint8_t margin;
			std::vector<Element> elms{};
		};

		RasterFont font;
		std::vector<Block> blocks{};

		Layout()
			: font(RasterFont::simple8())
		{
		}

		void draw(RenderTarget& rt);

		[[nodiscard]] Element* getElementAtPos(size_t ix, size_t iy);

#if SOUP_WINDOWS
		[[nodiscard]] Window createWindow(const std::string& title);
#endif
	};
}
