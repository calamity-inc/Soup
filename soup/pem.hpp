#pragma once

#include <string>

namespace soup
{
	struct pem
	{
		[[nodiscard]] static std::string decode(std::string in);
	};
}
