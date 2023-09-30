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
			// Not ideal because std::stoull may throw.
#if SOUP_EXCEPTIONS
			try
#endif
			{
				a = std::stoll(as);
				b = std::stoll(bs);
			}
#if SOUP_EXCEPTIONS
			catch (...)
			{
				return "Number between ... and ...?";
			}
#endif
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
