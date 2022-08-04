#pragma once

#include "lyoElement.hpp"

#include <string>

#include "RasterFont.hpp"

namespace soup
{
	struct lyoTextElement : public lyoElement
	{
		RasterFont font;
		std::u32string text;
		uint8_t scale = 1;

		lyoTextElement(lyoContainer* parent, std::u32string text)
			: lyoElement(parent), font(RasterFont::simple8()), text(std::move(text))
		{
			display_inline = true;
		}

		void flattenElement(lyoFlatDocument& flat) final;

		void draw(RenderTarget& rt) const final;
	};
}
