#pragma once

#include <memory>
#include <vector>

#include "conui_base.hpp"

namespace soup
{
	struct conui_div : public conui_base
	{
		std::vector<std::unique_ptr<conui_base>> children{};

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
