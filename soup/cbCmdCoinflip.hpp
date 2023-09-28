#pragma once

#include "cbCmd.hpp"

#include "rand.hpp"

namespace soup
{
	struct cbCmdCoinflip : public cbCmd
	{
		[[nodiscard]] bool checkTriggers(cbParser& p) const noexcept final
		{
			return p.checkTriggers({ "coinflip", "flip a coin" });
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			return soup::rand.coinflip() ? "Heads." : "Tails.";
		}
	};
}
