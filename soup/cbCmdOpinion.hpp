#pragma once

#include "cbCmd.hpp"

NAMESPACE_SOUP
{
	struct cbCmdOpinion : public cbCmd
	{
		[[nodiscard]] RegexMatchResult checkTriggers(const std::string& str) const final
		{
			static Regex r(R"(\b(?:do you think|do you like|your opinion)\b)");
			return r.search(str);
		}

		[[nodiscard]] cbResult process(const RegexMatchResult& m) const final
		{
			return "I don't have an opinion on that.";
		}
	};
}
