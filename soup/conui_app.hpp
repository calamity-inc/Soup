#pragma once

#include "conui_div.hpp"

#include <string>

namespace soup
{
	struct conui_app : public conui_div
	{
		conui_app();

		void draw() const override;

		void run();
	};
}
