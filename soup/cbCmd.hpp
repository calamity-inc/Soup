#pragma once

#include "cbResult.hpp"
#include "Regex.hpp"

namespace soup
{
	struct cbCmd
	{
		virtual ~cbCmd() = default;

		[[nodiscard]] virtual RegexMatchResult checkTriggers(const std::string& str) const = 0;
		[[nodiscard]] virtual cbResult process(const RegexMatchResult& m) const = 0;
	};
}
