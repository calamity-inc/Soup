#pragma once

#include <string>
#include <vector>

#include "aglStat.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	struct aglContent
	{
		std::vector<UniquePtr<aglStat>> statements{};

		[[nodiscard]] static aglContent fromSource(const std::string& code);

		void transpile(aglTranspiler& t) const;
		
		[[nodiscard]] std::string evaluate() const;
	};
}
