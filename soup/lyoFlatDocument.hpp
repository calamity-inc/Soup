#pragma once

#include "base.hpp"
#include "fwd.hpp"

#include <deque>

namespace soup
{
	struct lyoFlatDocument
	{
		std::deque<lyoElement*> elms; // "further back" elements must be at the top of the deque

		[[nodiscard]] bool isInitialised() const noexcept
		{
			return !elms.empty();
		}

		void draw(RenderTarget& rt) const;

		[[nodiscard]] lyoElement* getElementAtPos(unsigned int x, unsigned int y) const;
	};
}
