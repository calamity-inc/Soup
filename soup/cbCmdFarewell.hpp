#pragma once

#include "cbCmd.hpp"

namespace soup
{
	struct cbCmdFarewell : public cbCmd
	{
		[[nodiscard]] std::vector<std::string> getTriggers() const noexcept final
		{
			return { "bye", "goodbye", "farewell" };
		}

		[[nodiscard]] cbResult process(cbParser& p) const noexcept final
		{
			return "Goodbye!";
		}
	};
}
