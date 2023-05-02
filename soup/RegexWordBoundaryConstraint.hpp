#pragma once

#include "RegexConstraintTransitionable.hpp"

#include "RegexMatcher.hpp"
#include "string.hpp"

namespace soup
{
	struct RegexWordBoundaryConstraint : public RegexConstraintTransitionable
	{
		[[nodiscard]] bool matches(RegexMatcher& m) const noexcept final
		{
			if (m.it == m.begin
				|| m.it == m.end
				)
			{
				return true;
			}
			if (string::isWordChar(*(m.it - 1)))
			{
				return !string::isWordChar(*m.it);
			}
			else
			{
				return string::isWordChar(*m.it);
			}
		}

		[[nodiscard]] std::string toString() const noexcept final
		{
			return "\\b";
		}

		[[nodiscard]] size_t getCursorAdvancement() const final
		{
			return 0;
		}
	};
}
