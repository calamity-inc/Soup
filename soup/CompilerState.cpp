#include "CompilerState.hpp"

#include <algorithm>

namespace soup
{
	size_t CompilerState::getVarIndex(std::string key)
	{
		if (auto e = std::find(vars.begin(), vars.end(), key); e != vars.end())
		{
			return e - vars.begin();
		}
		size_t idx = vars.size();
		vars.emplace_back(std::move(key));
		return idx;
	}
}
