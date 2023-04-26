#pragma once

#include "RegexConstraintTransitionable.hpp"

#include "RegexMatcher.hpp"

namespace soup
{
	template <bool single_line>
	struct RegexAnyCharConstraint : public RegexConstraintTransitionable
	{
		[[nodiscard]] bool matches(RegexMatcher& m) const noexcept final
		{
			if (m.it == m.end)
			{
				return false;
			}
			if constexpr (!single_line)
			{
				if (*m.it == '\n')
				{
					return false;
				}
			}
			++m.it;
			return true;
		}

		[[nodiscard]] std::string toString() const noexcept final
		{
			return ".";
		}
	};
}
