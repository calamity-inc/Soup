#pragma once

#include "RegexConstraintTransitionable.hpp"

#include "UniquePtr.hpp"

namespace soup
{
	struct RegexGreedyOneConstraint : public RegexConstraintTransitionable
	{
		UniquePtr<RegexConstraint> constraint;

		RegexGreedyOneConstraint(UniquePtr<RegexConstraint>&& constraint)
			 : constraint(std::move(constraint))
		{
		}

		[[nodiscard]] bool matches(std::string::const_iterator& it, std::string::const_iterator end) const noexcept final
		{
			// Meta-constraint. Transitions will be set up to correctly handle matching of this.
			return true;
		}

		[[nodiscard]] std::string toString() const noexcept final
		{
			std::string str = constraint->toString();
			str.push_back('+');
			return str;
		}
	};
}
