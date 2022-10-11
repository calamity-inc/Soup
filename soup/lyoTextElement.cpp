#include "lyoTextElement.hpp"

#include "lyoContainer.hpp"
#include "lyoFlatDocument.hpp"
#include "RenderTarget.hpp"
#include "Rgb.hpp"

namespace soup
{
	void lyoTextElement::populateFlatDocument(lyoFlatDocument& fdoc)
	{
		fdoc.elms.emplace_back(this);
	}

	void lyoTextElement::updateFlatPos()
	{
		flat_x = parent->flat_x + style.margin_left;
		flat_y = parent->flat_y;

		if ((flat_x + flat_width >= parent->flat_width || !style.display_inline)
			&& flat_x != parent->style.margin_left
			)
		{
			flat_x = parent->style.margin_left;
			flat_y += flat_height + 3;
		}

		parent->flat_x = flat_x;
		parent->flat_y = flat_y;

		parent->flat_x += flat_width;
		parent->flat_x += ((font.get(' ').width * style.getFontScale()) + 1);
		parent->flat_x += style.margin_right;
	}

	void lyoTextElement::updateFlatSize()
	{
		auto [measured_width, measured_height] = font.measure(text);
		const auto scale = style.getFontScale();
		flat_width = measured_width * scale;
		flat_height = measured_height * scale;
	}

	void lyoTextElement::draw(RenderTarget& rt) const
	{
		rt.drawText(flat_x, flat_y, text, font, style.color.has_value() ? style.color.value() : Rgb::WHITE, style.getFontScale());
	}
}
