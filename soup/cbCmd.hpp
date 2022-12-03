#pragma once

#include <string>
#include <vector>

#include "cbParser.hpp"
#include "cbResult.hpp"

namespace soup
{
	struct cbCmd
	{
		virtual ~cbCmd() = default;

		[[nodiscard]] virtual std::vector<std::string> getTriggers() const noexcept = 0;

		[[nodiscard]] virtual cbResult process(cbParser& p) const noexcept = 0;
	};
}
