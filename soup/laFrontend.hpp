#pragma once

#include "irModule.hpp"

NAMESPACE_SOUP
{
	struct laFrontend
	{
		[[nodiscard]] virtual irModule parse(const std::string& program) = 0;
	};
}
