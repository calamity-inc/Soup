#pragma once

#include <istream>
#include <vector>

#include "zip_indexed_file.hpp"

namespace soup
{
	struct zip_reader
	{
		std::basic_istream<char, std::char_traits<char>>* is;

		zip_reader(std::basic_istream<char, std::char_traits<char>>* is)
			: is(is)
		{
		}

		[[nodiscard]] size_t seekCentralDirectory() const;

		[[nodiscard]] std::vector<zip_indexed_file> getFileList() const;

		// Note that no decompression will be performed, the file will be returned in the way it is stored.
		// So you might want to check if uncompressed_size == compressed_size.
		[[nodiscard]] std::string getFileContents(const zip_indexed_file& file) const;
	};
}
