#pragma once

#include "RegexGroup.hpp"

namespace soup
{
	struct Regex
	{
		RegexGroup group;

		Regex(const std::string& pattern)
			: group(pattern)
		{
		}

		Regex(const Regex& b)
			: Regex(b.toString())
		{
		}

		Regex(Regex&&) = default;

		[[nodiscard]] bool matches(const std::string& str) const noexcept;
		[[nodiscard]] bool matches(std::string::const_iterator it, std::string::const_iterator end) const noexcept;

		[[nodiscard]] std::string toString() const noexcept
		{
			return group.toString();
		}
	};
}
