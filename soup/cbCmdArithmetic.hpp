#pragma once

#include "cbCmd.hpp"

#include "MathExpr.hpp"

namespace soup
{
	struct cbCmdArithmetic : public cbCmd
	{
		[[nodiscard]] RegexMatchResult checkTriggers(const std::string& str) const final
		{
			static Regex r(R"((?:\d+\s*[+\-*\/%]\s*)+\d+)");
			return r.search(str);
		}

		[[nodiscard]] cbResult process(const RegexMatchResult& m) const final
		{
			std::string msg;
			if (int64_t val; MathExpr::evaluate(m.findGroupByIndex(0)->toString()).consume(val))
			{
				msg = "That expression evaluates to ";
				msg.append(std::to_string(val));
				msg.append(". :)");
			}
			else
			{
				msg = "Failed to evaluate the expression ";
				msg.append(m.findGroupByIndex(0)->toString());
			}
			return cbResult(std::move(msg));
		}
	};
}
