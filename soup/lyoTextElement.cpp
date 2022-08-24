#include "lyoTextElement.hpp"

#include "lyoContainer.hpp"
#include "lyoFlatDocument.hpp"
#include "RenderTarget.hpp"
#include "Rgb.hpp"

namespace soup
{
	void lyoTextElement::flattenElement(lyoFlatDocument& flat)
	{
		flat_x = parent->flat_x + style.margin_left;
		flat_y = parent->flat_y;

		const auto scale = style.getFontScale();
		auto [measured_width, measured_height] = font.measure(text);
		flat_width = measured_width * scale;
		flat_height = measured_height * scale;

		if (flat_x + flat_width >= parent->flat_width
			&& flat_x != parent->style.margin_left
			)
		{
			flat_x = parent->style.margin_left;
			flat_y += flat_height + 3;
		}

		flat.elms.emplace_back(this);

		parent->flat_x = flat_x;
		parent->flat_y = flat_y;

		parent->flat_x += flat_width;
		parent->flat_x += ((font.get(' ').width * scale) + 1);
		parent->flat_x += style.margin_right;
	}

	void lyoTextElement::draw(RenderTarget& rt) const
	{
		rt.drawText(flat_x, flat_y, text, font, Rgb::WHITE, style.getFontScale());
	}
}
