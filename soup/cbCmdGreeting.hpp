#pragma once

#include "cbCmd.hpp"

namespace soup
{
	struct cbCmdGreeting : public cbCmd
	{
		[[nodiscard]] bool checkTriggers(cbParser& p) const noexcept final
		{
			return p.checkTriggers({ "hi", "hey", "hello" });
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			return "Hello!";
		}
	};
}
