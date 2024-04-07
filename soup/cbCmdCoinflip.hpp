#pragma once

#include "cbCmd.hpp"

#include "rand.hpp"

NAMESPACE_SOUP
{
	struct cbCmdCoinflip : public cbCmd
	{
		[[nodiscard]] RegexMatchResult checkTriggers(const std::string& str) const final
		{
			static Regex r(R"(\b(?:coinflip|flip a coin)\b)");
			return r.search(str);
		}

		[[nodiscard]] cbResult process(const RegexMatchResult& m) const final
		{
			return soup::rand.coinflip() ? "Heads." : "Tails.";
		}
	};
}
