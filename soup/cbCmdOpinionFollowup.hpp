#pragma once

#include "cbCmd.hpp"

namespace soup
{
	struct cbCmdOpinionFollowup : public cbCmd
	{
		[[nodiscard]] std::vector<std::string> getTriggers() const noexcept final
		{
			return { "what do you have an opinion on" };
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			return "I am not capable of thought, therefore I have no opinions.";
		}
	};
}
