#pragma once

#include <vector>

#include "ConuiBase.hpp"
#include "UniquePtr.hpp"

NAMESPACE_SOUP
{
	struct ConuiDiv : public ConuiBase
	{
		std::vector<UniquePtr<ConuiBase>> children{};

		using ConuiBase::ConuiBase;

		void draw() const override
		{
			for (const auto& child : children)
			{
				child->draw();
			}
		}
	};
}
