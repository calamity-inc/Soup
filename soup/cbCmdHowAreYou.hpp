#pragma once

#include "cbCmd.hpp"

NAMESPACE_SOUP
{
	struct cbCmdHowAreYou : public cbCmd
	{
		[[nodiscard]] RegexMatchResult checkTriggers(const std::string& str) const final
		{
			static Regex r(R"(\b(?:how are you)\b)");
			return r.search(str);
		}

		[[nodiscard]] cbResult process(const RegexMatchResult& m) const final
		{
			return "Running smoothly.";
		}
	};
}
