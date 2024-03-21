#pragma once

#include "irModule.hpp"

namespace soup
{
	struct laBackend
	{
		[[nodiscard]] virtual std::string compileModule(const irModule& m) const = 0;
	};
}
