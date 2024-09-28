#pragma once

#include "cbCmd.hpp"

NAMESPACE_SOUP
{
	struct cbCmdCapabilities : public cbCmd
	{
		[[nodiscard]] RegexMatchResult checkTriggers(const std::string& str) const final
		{
			static Regex r(R"(\bwhat (?:can|do) you\b)", RE_INSENSITIVE);
			return r.search(str);
		}

		[[nodiscard]] cbResult process(const RegexMatchResult& m) const final
		{
			std::string msg = "I can do basic conversations, arithmetics, unit conversions (distance & weight)";
			if (cbCmdDefine::dict)
			{
				msg.append(", word definitions");
			}
			if (cbCmdJoke::jb)
			{
				msg.append(", joke telling");
			}
			msg.append(", and RNG (flip a coin, number between ...).");
			return cbResult(CB_RES_CAPABILITIES, std::move(msg));
		}
	};
}
