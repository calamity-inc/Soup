#pragma once

#include "RegexConstraint.hpp"

namespace soup
{
	struct RegexAnyCharConstraint : public RegexConstraint
	{
		[[nodiscard]] bool matches(std::string::const_iterator& it, std::string::const_iterator end) const noexcept final
		{
			if (it == end)
			{
				return false;
			}
			if (*it == '\n') // TODO: may also match '\n' if the 's' flag ("single line") is set.
			{
				return false;
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
