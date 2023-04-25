#pragma once

#include <string>

#include "fwd.hpp"

namespace soup
{
	struct RegexConstraint
	{
		const RegexConstraintTransitionable* success_transition = nullptr;
		const RegexConstraintTransitionable* rollback_transition = nullptr;
		const RegexGroup* group = nullptr;

		virtual ~RegexConstraint() = default;

		[[nodiscard]] virtual const RegexConstraintTransitionable* getTransition() const noexcept = 0;

		[[nodiscard]] virtual std::string toString() const noexcept = 0;
	};
}
