#pragma once

#include "cbCmd.hpp"

namespace soup
{
	struct cbCmdHowAreYou : public cbCmd
	{
		[[nodiscard]] bool checkTriggers(cbParser& p) const noexcept final
		{
			return p.checkTrigger("how are you");
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			return "Running smoothly.";
		}
	};
}
