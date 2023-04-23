#pragma once

#include <stack>
#include <string>

#include "fwd.hpp"
#include "Regex.hpp"

namespace soup
{
	struct RegexMatcher
	{
		struct RollbackPoint
		{
			const RegexConstraint* c;
			std::string::const_iterator it;
		};

		const RegexConstraint* c;
		std::string::const_iterator it;
		const std::string::const_iterator end;
		std::stack<RollbackPoint> rollback_points{};

		RegexMatcher(const Regex& r, std::string::const_iterator begin, std::string::const_iterator end)
			: c(r.group.initial), it(begin), end(end)
		{
		}

		void save(const RegexConstraint* rollback_transition) noexcept
		{
			rollback_points.push(RollbackPoint{ rollback_transition, it });
		}

		void restore()
		{
			c = rollback_points.top().c;
			it = rollback_points.top().it;
			rollback_points.pop();
		}
	};
}
