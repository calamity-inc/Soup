#pragma once

#include <ostream>
#include <vector>

#include "zip_indexed_file.hpp"

namespace soup
{
	class zip_writer
	{
	public:
		std::basic_ostream<char, std::char_traits<char>>* os;

		zip_writer(std::basic_ostream<char, std::char_traits<char>>* os)
			: os(os)
		{
		}

	protected:
		zip_indexed_file addFile(std::string name, const std::string& contents_uncompressed, uint16_t compression_method, const std::string& contents_compressed) const;
	public:
		zip_indexed_file addFileUncompressed(std::string name, const std::string& contents) const;
		zip_indexed_file addFileAnticompressed(std::string name, const std::string& contents_uncompressed) const;

		void finalise(const std::vector<zip_indexed_file>& files) const;
	};
}
