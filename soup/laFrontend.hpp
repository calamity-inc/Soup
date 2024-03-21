#pragma once

#include "irModule.hpp"

namespace soup
{
	struct laFrontend
	{
		[[nodiscard]] virtual irModule parse(const std::string& program) = 0;
	};
}
