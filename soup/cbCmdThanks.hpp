#pragma once

#include "cbCmd.hpp"

NAMESPACE_SOUP
{
	struct cbCmdThanks : public cbCmd
	{
		[[nodiscard]] RegexMatchResult checkTriggers(const std::string& str) const final
		{
			static Regex r(R"(\b(?:thanks|thank you)\b)");
			return r.search(str);
		}

		[[nodiscard]] cbResult process(const RegexMatchResult& m) const final
		{
			return "You're welcome!";
		}
	};
}
