#pragma once

#include <string>

#include "fwd.hpp"

namespace soup
{
	struct RegexConstraint
	{
		const RegexConstraint* success_transition = nullptr;
		const RegexConstraint* rollback_transition = nullptr;

		virtual ~RegexConstraint() = default;

		[[nodiscard]] virtual const RegexConstraint* getTransition() const noexcept
		{
			return this;
		}

		// May only modify `it` if the constraint matches.
		[[nodiscard]] virtual bool matches(std::string::const_iterator& it, std::string::const_iterator end) const noexcept = 0;

		[[nodiscard]] virtual std::string toString() const noexcept = 0;
	};
}
