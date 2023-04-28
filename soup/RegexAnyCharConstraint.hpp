#pragma once

#include "RegexConstraintTransitionable.hpp"

#include "RegexMatcher.hpp"

namespace soup
{
	template <bool dotall>
	struct RegexAnyCharConstraint : public RegexConstraintTransitionable
	{
		[[nodiscard]] bool matches(RegexMatcher& m) const noexcept final
		{
			if (m.it == m.end)
			{
				return false;
			}
			if constexpr (!dotall)
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

		void getFlags(uint16_t& set, uint16_t& unset) const noexcept final
		{
			if constexpr (dotall)
			{
				set |= RE_DOTALL;
			}
			else
			{
				unset |= RE_DOTALL;
			}
		}
	};
}
