#pragma once

#include <string>

#include "fwd.hpp"

#include "Exception.hpp"

namespace soup
{
	struct RegexConstraint
	{
		const RegexConstraintTransitionable* success_transition = nullptr;
		const RegexConstraintTransitionable* rollback_transition = nullptr;
		const RegexGroup* group = nullptr;

		virtual ~RegexConstraint() = default;

		[[nodiscard]] virtual const RegexConstraintTransitionable* getTransition() const noexcept = 0;

		[[nodiscard]] virtual size_t getCursorAdvancement() const
		{
			throw Exception("Constraint is not fixed-width");
		}

		[[nodiscard]] virtual UniquePtr<RegexConstraint> clone() const
		{
			throw Exception("Constraint is not clonable");
		}

		[[nodiscard]] virtual std::string toString() const noexcept = 0;

		virtual void getFlags(uint16_t& set, uint16_t& unset) const noexcept {}
	};
}
