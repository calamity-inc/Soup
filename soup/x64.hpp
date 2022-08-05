#pragma once

#include <cstdint>
#include <string>

namespace soup
{
	struct x64
	{
		[[nodiscard]] static std::string disasm(const uint8_t*& code);
	};
}
