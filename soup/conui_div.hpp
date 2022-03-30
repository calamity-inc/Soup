#pragma once

#include <vector>

#include "conui_base.hpp"
#include "unique_ptr.hpp"

namespace soup
{
	struct conui_div : public conui_base
	{
		std::vector<unique_ptr<conui_base>> children{};

		using conui_base::conui_base;

		void draw() const override
		{
			for (const auto& child : children)
			{
				child->draw();
			}
		}
	};
}
