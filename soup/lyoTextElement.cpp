#include "lyoTextElement.hpp"

#include "lyoContainer.hpp"
#include "RenderTarget.hpp"
#include "Rgb.hpp"

#if LYO_DEBUG_POS
#include "format.hpp"
#include "log.hpp"
#include "unicode.hpp"
#endif

namespace soup
{
	void lyoTextElement::updateFlatSize()
	{
		auto [measured_width, measured_height] = font.measure(text);
		const auto scale = style.getFontScale();
		flat_width = measured_width * scale;
		flat_height = measured_height * scale;
	}

	void lyoTextElement::updateFlatPos(unsigned int& x, unsigned int& y, unsigned int& wrap_y)
	{
#if LYO_DEBUG_POS
		logWriteLine(format("lyoTextElement({})::updateFlatPos - Start: {}, {}", unicode::utf32_to_utf8(text), x, y));
#endif

		// Update wrap_y
		wrap_y = (y + flat_height);

		// Check line wrap
		if (x + flat_width >= parent->flat_width
			|| style.display_block
			)
		{
			wrapLine(x, y, wrap_y);

#if LYO_DEBUG_POS
			logWriteLine(format("lyoTextElement({})::updateFlatPos - Wrap: {}, {}", unicode::utf32_to_utf8(text), x, y));
#endif
		}

		// Done positioning this text element
		setFlatPos(x, y);

		// Update x
		x += flat_width;
		x += style.margin_right;
		if (parent->tag_name == "span")
		{
			x += (font.get(' ').width * style.getFontScale());
		}
		else
		{
			x += style.getFontScale();
		}

#if LYO_DEBUG_POS
		logWriteLine(format("lyoTextElement({})::updateFlatPos - End: {}, {}", unicode::utf32_to_utf8(text), x, y));
#endif
	}

	void lyoTextElement::draw(RenderTarget& rt) const
	{
		lyoElement::draw(rt);
		//rt.drawRect(flat_x, flat_y, flat_width, flat_height, Rgb::GREEN);
		rt.drawText(flat_x, flat_y, text, font, style.color.has_value() ? style.color.value() : Rgb::WHITE, style.getFontScale());
	}
}
