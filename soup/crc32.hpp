#pragma once

#include <cstdint>
#include <string>

namespace soup
{
	struct crc32
	{
		static uint32_t hash(const std::string& data);
	};
}
