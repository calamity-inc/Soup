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

		lyoTextElement(lyoContainer* parent, std::u32string text)
			: lyoElement(parent), font(RasterFont::simple8()), text(std::move(text))
		{
		}

		void populateFlatDocument(lyoFlatDocument& fdoc) final;
		void updateFlatPos() final;
		void updateFlatSize() final;

		void draw(RenderTarget& rt) const final;
	};
}
