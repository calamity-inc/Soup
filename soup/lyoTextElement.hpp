#pragma once

#include "lyoElement.hpp"

#include <string>

#include "RasterFont.hpp"

NAMESPACE_SOUP
{
	struct lyoTextElement : public lyoElement
	{
		const RasterFont* font;
		std::u32string text;

		lyoTextElement(lyoContainer* parent, std::u32string text)
			: lyoElement(parent), font(&RasterFont::simple8()), text(std::move(text))
		{
		}

		void updateFlatSize() final;
		void updateFlatPos(unsigned int& x, unsigned int& y, unsigned int& wrap_y) final;

		void draw(RenderTarget& rt) const final;
	};
}
