#pragma once

#include "cbCmd.hpp"

#include "rand.hpp"

namespace soup
{
	struct cbCmdRand : public cbCmd
	{
		[[nodiscard]] bool checkTriggers(cbParser& p) const noexcept final
		{
			return p.checkTrigger("number between");
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			auto as = p.getArgNumeric();
			auto bs = p.getArgNumericSecond();
			long long a, b;
			try
			{
				a = std::stoll(as);
				b = std::stoll(bs);
			}
			catch (...)
			{
				return "Number between ... and ...?";
			}
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
