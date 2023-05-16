#pragma once

#include <string>

namespace soup
{
	struct RegexMatchedGroup
	{
		std::string name;
		std::string::const_iterator begin;
		std::string::const_iterator end;

		[[nodiscard]] size_t length() const
		{
			return std::distance(begin, end);
		}

		[[nodiscard]] std::string toString() const noexcept
		{
			return std::string(begin, end);
		}
	};
}
