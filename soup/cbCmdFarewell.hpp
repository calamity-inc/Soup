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

		[[nodiscard]] std::string getResponse(cbParser& p) const noexcept final
		{
			return "Goodbye!";
		}
	};
}
