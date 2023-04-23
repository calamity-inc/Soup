#pragma once

#include "RegexConstraint.hpp"

#include "RegexGroup.hpp"

namespace soup
{
	struct RegexGroupConstraint : public RegexConstraint
	{
		RegexGroup group;

		RegexGroupConstraint(std::string::const_iterator& it, std::string::const_iterator end)
			: group(it, end)
		{
		}

		[[nodiscard]] const RegexConstraint* getTransition() const noexcept final
		{
			return group.initial;
		}

		[[nodiscard]] const RegexConstraint** getSuccessTransitionPointer() const noexcept
		{
			return &group.alternatives.back().constraints.back()->success_transition;
		}

		[[nodiscard]] bool matches(std::string::const_iterator& it, std::string::const_iterator end) const noexcept final
		{
			// Group constraints cannot be transitioned to, therefore this function will never be called.
			return false;
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
