#pragma once

#include "cbCmd.hpp"

namespace soup
{
	struct cbCmdGreeting : public cbCmd
	{
		[[nodiscard]] std::vector<std::string> getTriggers() const noexcept final
		{
			return { "hi", "hey", "hello" };
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			return "Hello!";
		}
	};
}
