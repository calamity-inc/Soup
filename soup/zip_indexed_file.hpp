#pragma once

#include <cstdint>
#include <string>

namespace soup
{
	struct zip_indexed_file
	{
		uint32_t uncompressed_data_crc32;
		uint32_t compressed_size;
		uint32_t uncompressed_size;
		std::string name;
		uint32_t disk_offset;
	};
}
