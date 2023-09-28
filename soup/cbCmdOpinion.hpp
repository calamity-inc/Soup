#pragma once

#include "cbCmd.hpp"

namespace soup
{
	struct cbCmdOpinion : public cbCmd
	{
		[[nodiscard]] bool checkTriggers(cbParser& p) const noexcept final
		{
			return p.checkTriggers({ "do you think", "do you like", "your opinion" });
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			return "I don't have an opinion on that.";
		}
	};
}
