#pragma once

#include "cbCmd.hpp"

namespace soup
{
	struct cbCmdThanks : public cbCmd
	{
		[[nodiscard]] bool checkTriggers(cbParser& p) const noexcept final
		{
			return p.checkTriggers({ "thanks", "thank you" });
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			return "You're welcome!";
		}
	};
}
