#pragma once

#include "RegexConstraint.hpp"

#include "RegexGroup.hpp"

namespace soup
{
	struct RegexGroupConstraint : public RegexConstraint
	{
		RegexGroup group;

		RegexGroupConstraint(const RegexGroup::ConstructorState& s)
			: group(s)
		{
		}

		[[nodiscard]] const RegexConstraintTransitionable* getTransition() const noexcept final
		{
			return group.initial;
		}

		[[nodiscard]] std::string toString() const noexcept final
		{
			auto str = group.toString();
			str.insert(0, 1, '(');
			str.push_back(')');
			return str;
		}
	};
}
