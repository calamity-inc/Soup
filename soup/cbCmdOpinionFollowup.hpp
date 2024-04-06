#pragma once

#include "cbCmd.hpp"

namespace soup
{
	struct cbCmdOpinionFollowup : public cbCmd
	{
		[[nodiscard]] RegexMatchResult checkTriggers(const std::string& str) const final
		{
			static Regex r(R"(\b(?:what do you have an opinion on)\b)");
			return r.search(str);
		}

		[[nodiscard]] cbResult process(const RegexMatchResult& m) const final
		{
			return "I am not capable of thought, therefore I have no opinions.";
		}
	};
}
