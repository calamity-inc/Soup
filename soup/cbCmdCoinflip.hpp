#pragma once

#include "cbCmd.hpp"

#include "rand.hpp"

namespace soup
{
	struct cbCmdCoinflip : public cbCmd
	{
		[[nodiscard]] std::vector<std::string> getTriggers() const noexcept final
		{
			return { "coinflip", "flip a coin" };
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			return soup::rand.coinflip() ? "Heads." : "Tails.";
		}
	};
}
