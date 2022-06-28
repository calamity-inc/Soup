#pragma once

#include "fwd.hpp"

#include <string>
#include <vector>

namespace soup
{
	struct CompilerState
	{
		Writer* w;
		std::vector<std::string> vars{};

		explicit CompilerState(Writer* w)
			: w(w)
		{
		}

		[[nodiscard]] size_t getVarIndex(std::string key);
	};
}
