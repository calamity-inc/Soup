#pragma once

#include "RegexConstraintTransitionable.hpp"

namespace soup
{
	struct RegexDummyConstraint : public RegexConstraintTransitionable
	{
		using RegexConstraintTransitionable::RegexConstraintTransitionable;

		[[nodiscard]] size_t getCursorAdvancement() const final
		{
			return 0;
		}

		[[nodiscard]] std::string toString() const noexcept final
		{
			return {};
		}

		[[nodiscard]] bool matches(RegexMatcher& m) const noexcept final
		{
			return true;
		}
	};
}
