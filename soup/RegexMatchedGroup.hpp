#pragma once

#include <string>

namespace soup
{
	struct RegexMatchedGroup
	{
		std::string name;
		std::string::const_iterator begin;
		std::string::const_iterator end;

		[[nodiscard]] std::string toString() const noexcept
		{
			return std::string(begin, end);
		}
	};
}
