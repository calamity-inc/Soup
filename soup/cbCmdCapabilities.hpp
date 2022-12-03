#pragma once

#include "cbCmd.hpp"

namespace soup
{
	struct cbCmdCapabilities : public cbCmd
	{
		[[nodiscard]] std::vector<std::string> getTriggers() const noexcept final
		{
			return { "what can you", "what do you" };
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
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
