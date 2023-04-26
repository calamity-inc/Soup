#pragma once

#include <optional>
#include <stack>
#include <string>
#include <vector>

#include "fwd.hpp"
#include "Regex.hpp"
#include "RegexMatchedGroup.hpp"

namespace soup
{
	struct RegexMatcher
	{
		struct RollbackPoint
		{
			const RegexConstraintTransitionable* c;
			std::string::const_iterator it;
		};

		const RegexConstraintTransitionable* c;
		std::string::const_iterator it;
		const std::string::const_iterator begin;
		const std::string::const_iterator end;
		std::stack<RollbackPoint> rollback_points{};
		std::vector<std::optional<RegexMatchedGroup>> groups{};

		RegexMatcher(const Regex& r, std::string::const_iterator begin, std::string::const_iterator end)
			: c(r.group.initial), it(begin), begin(begin), end(end)
		{
		}

		void save(const RegexConstraintTransitionable* rollback_transition) noexcept
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
