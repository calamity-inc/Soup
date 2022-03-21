#pragma once

#include "conui_base.hpp"

#include <string>

#include "rgb.hpp"

namespace soup
{
	struct conui_span : public conui_base
	{
		rgb fg_colour;
		rgb bg_colour;
		std::string text;

		conui_span(conui_div* parent, unsigned int x, unsigned int y, std::string&& text = {}, rgb fg_colour = rgb::WHITE, rgb bg_colour = rgb::BLACK);

		void draw() const final;

		void setText(std::string&& text);
	};
}
