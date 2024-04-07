#pragma once

#include "ConuiDiv.hpp"

#include <string>

NAMESPACE_SOUP
{
	struct ConuiApp : public ConuiDiv
	{
		ConuiApp();

		void draw() const override;

		void run();
	};
}
