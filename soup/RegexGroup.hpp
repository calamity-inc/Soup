#pragma once

#include <string>
#include <vector>

#include "RegexAlternative.hpp"

namespace soup
{
	struct RegexGroup
	{
		struct ConstructorState
		{
			mutable std::string::const_iterator it;
			std::string::const_iterator end;
			mutable size_t next_index = 0;
			mutable std::vector<const RegexConstraintTransitionable**> alternatives_transitions{};

			ConstructorState(std::string::const_iterator it, std::string::const_iterator end)
				: it(it), end(end)
			{
			}
		};

		const size_t index = 0;
		const RegexGroup* parent = nullptr;
		const RegexConstraintTransitionable* initial = nullptr;
		std::vector<RegexAlternative> alternatives{};

		RegexGroup(const std::string& pattern)
			: RegexGroup(pattern.cbegin(), pattern.cend())
		{
		}

		RegexGroup(std::string::const_iterator it, std::string::const_iterator end)
			: RegexGroup(ConstructorState(it, end))
		{
		}

		RegexGroup(const ConstructorState& s, bool non_capturing = false);

		[[nodiscard]] std::string toString() const noexcept;
	};
}
