#pragma once

#include <cstddef> // size_t
#include <string>
#include <vector>

namespace soup
{
	struct EditorFile
	{
		std::vector<std::u32string> contents;
		size_t x = 0;
		size_t y = 0;

		[[nodiscard]] size_t getLineWidth() const
		{
			return contents.at(y).size();
		}
	};
}
