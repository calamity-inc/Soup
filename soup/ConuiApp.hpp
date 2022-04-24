#pragma once

#include "ConuiDiv.hpp"

#include <string>

namespace soup
{
	struct ConuiApp : public ConuiDiv
	{
		ConuiApp();

		void draw() const override;

		void run();
	};
}
