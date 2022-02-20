#pragma once

#include <string>

namespace soup
{
	struct pem
	{
		[[nodiscard]] static std::string encode(const std::string& label, const std::string& bin);
		[[nodiscard]] static std::string decode(std::string in);
	};
}
