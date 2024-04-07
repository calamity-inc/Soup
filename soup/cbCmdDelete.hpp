#pragma once

#include "cbCmd.hpp"

NAMESPACE_SOUP
{
	struct cbCmdDelete : public cbCmd
	{
		[[nodiscard]] RegexMatchResult checkTriggers(const std::string& str) const final
		{
			static Regex r(R"(\bdelete.+?(?'amount'\d+))");
			return r.search(str);
		}

		[[nodiscard]] cbResult process(const RegexMatchResult& m) const final
		{
			cbResult res(CB_RES_DELETE);
			res.extra = string::toInt<int64_t>(m.findGroupByName("amount")->toString(), 0);
			return res;
		}
	};
}
