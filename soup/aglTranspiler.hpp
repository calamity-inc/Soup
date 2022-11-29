#pragma once

#include <cstddef>
#include <set>
#include <string>

#include "aglLang.hpp"

namespace soup
{
	struct aglTranspiler
	{
		const aglLang target;

		std::set<std::string> includes{};
		std::string body{};

		size_t indentation = 0;

		explicit aglTranspiler(aglLang target)
			: target(target)
		{
		}

		[[nodiscard]] std::string getOutput() const;

		void indent();
	};
}
