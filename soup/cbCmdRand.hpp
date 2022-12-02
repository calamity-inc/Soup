#pragma once

#include "cbCmd.hpp"

#include "rand.hpp"

namespace soup
{
	struct cbCmdRand : public cbCmd
	{
		[[nodiscard]] std::vector<std::string> getTriggers() const noexcept final
		{
			return { "number between" };
		}

		[[nodiscard]] std::string getResponse(cbParser& p) const noexcept final
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
			return msg;
		}
	};
}
