#pragma once

#include "cbParser.hpp"
#include "cbResult.hpp"

namespace soup
{
	struct cbCmd
	{
		virtual ~cbCmd() = default;

		[[nodiscard]] virtual bool checkTriggers(cbParser& p) const noexcept = 0;
		[[nodiscard]] virtual cbResult process(cbParser& p) const noexcept = 0;
	};
}
