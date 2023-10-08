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
			if (int64_t a; string::toInt<int64_t, string::TI_FULL>(as).consume(a))
			{
				auto bs = p.getArgNumericSecond();
				if (int64_t b; string::toInt<int64_t, string::TI_FULL>(bs).consume(b))
				{
					if (a > b)
					{
						std::swap(a, b);
					}
					std::string msg = std::to_string(soup::rand.t<long long>(a, b));
					msg.push_back('.');
					return cbResult(std::move(msg));
				}
			}
			return "Number between ... and ...?";
		}
	};
}
