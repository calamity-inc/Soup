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
		[[nodiscard]] std::string getFileContents(const zip_indexed_file& file) const;
	};
}
