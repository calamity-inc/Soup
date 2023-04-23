#pragma once

#include <string>
#include <vector>

#include "RegexAlternative.hpp"

namespace soup
{
	struct RegexGroup
	{
		const RegexConstraint* initial = nullptr;
		std::vector<RegexAlternative> alternatives{};

		struct NonRefTag {};

		RegexGroup(const std::string& pattern)
			: RegexGroup(pattern.cbegin(), pattern.cend(), NonRefTag{})
		{
		}

		RegexGroup(std::string::const_iterator it, std::string::const_iterator end, NonRefTag)
			: RegexGroup(static_cast<std::string::const_iterator&>(it), end)
		{
		}

		RegexGroup(std::string::const_iterator& it, std::string::const_iterator end);

		[[nodiscard]] std::string toString() const noexcept;
	};
}
