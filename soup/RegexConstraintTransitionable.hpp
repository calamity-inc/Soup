#pragma once

#include "RegexConstraint.hpp"

namespace soup
{
	struct RegexConstraintTransitionable : public RegexConstraint
	{
		[[nodiscard]] const RegexConstraintTransitionable* getTransition() const noexcept final
		{
			return this;
		}

		// May only modify `it` if the constraint matches.
		[[nodiscard]] virtual bool matches(std::string::const_iterator& it, std::string::const_iterator end) const noexcept = 0;
	};
}
