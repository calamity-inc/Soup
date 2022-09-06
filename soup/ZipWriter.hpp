#pragma once

#include <ostream>
#include <vector>

#include "ZipIndexedFile.hpp"

namespace soup
{
	class ZipWriter
	{
	public:
		std::ostream& os;

		ZipWriter(std::ostream& os)
			: os(os)
		{
		}

	protected:
		ZipIndexedFile addFile(std::string name, const std::string& contents_uncompressed, uint16_t compression_method, const std::string& contents_compressed) const;
	public:
		ZipIndexedFile addFileUncompressed(std::string name, const std::string& contents) const;
		ZipIndexedFile addFileAnticompressed(std::string name, const std::string& contents_uncompressed) const;

		void finalise(const std::vector<ZipIndexedFile>& files) const;
	};
}
