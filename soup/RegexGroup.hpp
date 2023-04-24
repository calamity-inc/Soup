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
			mutable std::vector<const RegexConstraint**> alternatives_transitions{};

			ConstructorState(std::string::const_iterator it, std::string::const_iterator end)
				: it(it), end(end)
			{
			}
		};

		const RegexConstraint* initial = nullptr;
		std::vector<RegexAlternative> alternatives{};

		RegexGroup(const std::string& pattern)
			: RegexGroup(pattern.cbegin(), pattern.cend())
		{
		}

		RegexGroup(std::string::const_iterator it, std::string::const_iterator end)
			: RegexGroup(ConstructorState(it, end))
		{
		}

		RegexGroup(const ConstructorState& s);

		[[nodiscard]] std::string toString() const noexcept;
	};
}
