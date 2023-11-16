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
			std::vector<std::optional<RegexMatchedGroup>> groups{};
		};

		const RegexConstraintTransitionable* c;
		std::string::const_iterator it;
		const std::string::const_iterator begin;
		const std::string::const_iterator end;
		std::stack<RollbackPoint> rollback_points{};
		std::stack<std::string::const_iterator> checkpoints{};
		std::vector<std::optional<RegexMatchedGroup>> groups{};

		RegexMatcher(const Regex& r, std::string::const_iterator it, std::string::const_iterator begin, std::string::const_iterator end)
			: c(r.group.initial), it(it), begin(begin), end(end)
		{
		}

		void saveRollback(const RegexConstraintTransitionable* rollback_transition)
		{
			rollback_points.push(RollbackPoint{ rollback_transition, it, groups });
		}

		void restoreRollback()
		{
			c = rollback_points.top().c;
			it = rollback_points.top().it;
			groups = std::move(rollback_points.top().groups);
			rollback_points.pop();
		}

		void saveCheckpoint()
		{
			checkpoints.push(it);
		}

		void restoreCheckpoint()
		{
			it = checkpoints.top();
			checkpoints.pop();
		}
	};
}
