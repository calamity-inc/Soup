#pragma once

#include "ConuiBase.hpp"

#include <string>

#include "Rgb.hpp"

namespace soup
{
	struct ConuiSpan : public ConuiBase
	{
		Rgb fg_colour;
		Rgb bg_colour;
		std::string text;

		ConuiSpan(ConuiDiv* parent, unsigned int x, unsigned int y, std::string&& text = {}, Rgb fg_colour = Rgb::WHITE, Rgb bg_colour = Rgb::BLACK);

		void draw() const final;

		void setText(std::string&& text);
	};
}
