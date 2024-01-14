#pragma once

#include <optional>
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
			const char* it;
			std::vector<std::optional<RegexMatchedGroup>> groups{};
		};

		const RegexConstraintTransitionable* c;
		const char* it;
		const char* const begin;
		const char* const end;
		std::vector<RollbackPoint> rollback_points{};
		std::vector<const char*> checkpoints{};
		std::vector<std::optional<RegexMatchedGroup>> groups{};

		RegexMatcher(const Regex& r, const char* it, const char* begin, const char* end)
			: c(r.group.initial), it(it), begin(begin), end(end)
		{
		}

		void saveRollback(const RegexConstraintTransitionable* rollback_transition)
		{
			rollback_points.emplace_back(RollbackPoint{ rollback_transition, it, groups });
		}

		void restoreRollback()
		{
			c = rollback_points.back().c;
			it = rollback_points.back().it;
			groups = std::move(rollback_points.back().groups);
			rollback_points.pop_back();
		}

		void saveCheckpoint()
		{
			checkpoints.emplace_back(it);
		}

		void restoreCheckpoint()
		{
			it = checkpoints.back();
			checkpoints.pop_back();
		}
	};
}
