#pragma once

#include "RegexConstraint.hpp"

namespace soup
{
	struct RegexConstraintTransitionable : public RegexConstraint
	{
		[[nodiscard]] const RegexConstraintTransitionable* getTransition() const noexcept override
		{
			return this;
		}

		// May only modify `m.it` and only if the constraint matches.
		[[nodiscard]] virtual bool matches(RegexMatcher& m) const noexcept = 0;
	};
}
