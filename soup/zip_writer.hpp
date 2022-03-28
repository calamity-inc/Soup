#pragma once

#include <ostream>
#include <vector>

#include "zip_indexed_file.hpp"

namespace soup
{
	struct zip_writer
	{
		std::basic_ostream<char, std::char_traits<char>>* os;

		zip_writer(std::basic_ostream<char, std::char_traits<char>>* os)
			: os(os)
		{
		}

		// Note that no compression will be performed.
		zip_indexed_file addFile(std::string name, const std::string& contents) const;

		void finalise(const std::vector<zip_indexed_file>& files) const;
	};
}
