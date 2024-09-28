#pragma once

#include "cbCmd.hpp"

NAMESPACE_SOUP
{
	struct cbCmdFarewell : public cbCmd
	{
		[[nodiscard]] RegexMatchResult checkTriggers(const std::string& str) const final
		{
			static Regex r(R"(\b(?:bye|goodbye|farewell)\b)", RE_INSENSITIVE);
			return r.search(str);
		}

		[[nodiscard]] cbResult process(const RegexMatchResult& m) const final
		{
			return "Goodbye!";
		}
	};
}
