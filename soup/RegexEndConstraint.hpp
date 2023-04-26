#pragma once

#include "RegexConstraintTransitionable.hpp"

namespace soup
{
	template <bool multi_line, bool dollar_end_only>
	struct RegexEndConstraint : public RegexConstraintTransitionable
	{
		[[nodiscard]] bool matches(RegexMatcher& m) const noexcept final
		{
			if (m.it == m.end)
			{
				return true;
			}
			if constexpr (multi_line)
			{
				if (*m.it == '\n')
				{
					return true;
				}
			}
			else if constexpr (!dollar_end_only)
			{
				if ((m.it + 1) == m.end
					&& *m.it == '\n'
					)
				{
					return true;
				}
			}
			return false;
		}

		[[nodiscard]] std::string toString() const noexcept final
		{
			return "$";
		}

		void getFlags(uint16_t& set, uint16_t& unset) const noexcept final
		{
			if constexpr (multi_line)
			{
				set |= Regex::multi_line;
			}
			else
			{
				unset |= Regex::multi_line;
			}
			if constexpr (dollar_end_only)
			{
				set |= Regex::dollar_end_only;
			}
			else
			{
				unset |= Regex::dollar_end_only;
			}
		}
	};
}
