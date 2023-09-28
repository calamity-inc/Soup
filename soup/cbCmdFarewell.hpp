#pragma once

#include "cbCmd.hpp"

namespace soup
{
	struct cbCmdFarewell : public cbCmd
	{
		[[nodiscard]] bool checkTriggers(cbParser& p) const noexcept final
		{
			return p.checkTriggers({ "bye", "goodbye", "farewell" });
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			return "Goodbye!";
		}
	};
}
