#pragma once

#include "cbCmd.hpp"

namespace soup
{
	struct cbCmdThanks : public cbCmd
	{
		[[nodiscard]] std::vector<std::string> getTriggers() const noexcept final
		{
			return { "thanks", "thank you" };
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			return "You're welcome!";
		}
	};
}
