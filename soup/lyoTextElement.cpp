#include "lyoTextElement.hpp"

#include "lyoContainer.hpp"
#include "RenderTarget.hpp"
#include "Rgb.hpp"

namespace soup
{
	void lyoTextElement::updateFlatPos()
	{
		lyoElement::updateFlatPos();

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
		lyoElement::draw(rt);
		rt.drawText(flat_x, flat_y, text, font, style.color.has_value() ? style.color.value() : Rgb::WHITE, style.getFontScale());
	}
}
