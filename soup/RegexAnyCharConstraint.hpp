#pragma once

#include "RegexConstraintTransitionable.hpp"

namespace soup
{
	template <bool single_line>
	struct RegexAnyCharConstraint : public RegexConstraintTransitionable
	{
		[[nodiscard]] bool matches(std::string::const_iterator& it, std::string::const_iterator end) const noexcept final
		{
			if (it == end)
			{
				return false;
			}
			if constexpr (!single_line)
			{
				if (*it == '\n')
				{
					return false;
				}
			}
			++it;
			return true;
		}

		[[nodiscard]] std::string toString() const noexcept final
		{
			return ".";
		}
	};
}
