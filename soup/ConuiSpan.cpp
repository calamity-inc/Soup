#include "ConuiSpan.hpp"

#include "console.hpp"
#include "unicode.hpp"

namespace soup
{
	ConuiSpan::ConuiSpan(ConuiDiv* parent, unsigned int x, unsigned int y, std::string&& text, Rgb fg_colour, Rgb bg_colour)
		: ConuiBase(parent, x, y, static_cast<unsigned int>(unicode::utf8_char_len(text)), 1), fg_colour(fg_colour), bg_colour(bg_colour), text(std::move(text))
	{
	}

	void ConuiSpan::draw() const
	{
		console.setCursorPos(x, y);
		console.setForegroundColour(fg_colour);
		console.setBackgroundColour(bg_colour);
		console << text;
	}

	void ConuiSpan::setText(std::string&& text)
	{
		this->width = static_cast<unsigned int>(unicode::utf8_char_len(text));
		this->text = std::move(text);
		reinterpret_cast<ConuiBase*>(getApp())->draw();
	}
}
