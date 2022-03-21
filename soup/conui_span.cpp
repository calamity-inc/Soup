#include "conui_span.hpp"

#include "console.hpp"
#include "unicode.hpp"

namespace soup
{
	conui_span::conui_span(conui_div* parent, unsigned int x, unsigned int y, std::string&& text, rgb fg_colour, rgb bg_colour)
		: conui_base(parent, x, y, unicode::utf8_char_len(text), 1), fg_colour(fg_colour), bg_colour(bg_colour), text(std::move(text))
	{
	}

	void conui_span::draw() const
	{
		console.setCursorPos(x, y);
		console.setForegroundColour(fg_colour);
		console.setBackgroundColour(bg_colour);
		console << text;
	}

	void conui_span::setText(std::string&& text)
	{
		this->width = unicode::utf8_char_len(text);
		this->text = std::move(text);
		reinterpret_cast<conui_base*>(getApp())->draw();
	}
}
