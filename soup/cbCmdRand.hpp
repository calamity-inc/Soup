#pragma once

#include "cbCmd.hpp"

#include "rand.hpp"

NAMESPACE_SOUP
{
	struct cbCmdRand : public cbCmd
	{
		[[nodiscard]] RegexMatchResult checkTriggers(const std::string& str) const final
		{
			static Regex r(R"(\bnumber between\s*(?'a'\d+).+?(?'b'\d+)\b)");
			return r.search(str);
		}

		[[nodiscard]] cbResult process(const RegexMatchResult& m) const final
		{
			auto a = string::toInt<int64_t>(m.findGroupByName("a")->toString(), 0);
			auto b = string::toInt<int64_t>(m.findGroupByName("b")->toString(), 0);
			if (a > b)
			{
				std::swap(a, b);
			}
			std::string msg = std::to_string(soup::rand.t<long long>(a, b));
			msg.push_back('.');
			return cbResult(std::move(msg));
		}
	};
}
