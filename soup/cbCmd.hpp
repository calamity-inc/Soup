#pragma once

#include <string>
#include <vector>

#include "cbParser.hpp"

namespace soup
{
	struct cbCmd
	{
		virtual ~cbCmd() = default;

		[[nodiscard]] virtual std::vector<std::string> getTriggers() const noexcept = 0;

		[[nodiscard]] virtual std::string getResponse(cbParser& p) const noexcept = 0;
	};
}
