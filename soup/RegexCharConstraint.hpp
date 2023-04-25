#pragma once

#include "RegexConstraintTransitionable.hpp"

namespace soup
{
	struct RegexCharConstraint : public RegexConstraintTransitionable
	{
		char c;

		RegexCharConstraint(char c)
			: c(c)
		{
		}

		[[nodiscard]] bool matches(std::string::const_iterator& it, std::string::const_iterator end) const noexcept final
		{
			if (it == end)
			{
				return false;
			}
			if (*it != c)
			{
				return false;
			}
			++it;
			return true;
		}

		[[nodiscard]] std::string toString() const noexcept final
		{
			std::string str(1, c);
			switch (c)
			{
			case '\\':
			case '|':
			case '(':
			case ')':
			case '?':
			case '.':
				str.insert(0, 1, '\\');
				break;
			}
			return str;
		}
	};
}
