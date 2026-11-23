#pragma once

#include <vector>

#include "fwd.hpp"
#include "ZipIndexedFile.hpp"

NAMESPACE_SOUP
{
	class ZipWriter
	{
	public:
		Writer& os;

		ZipWriter(Writer& os)
			: os(os)
		{
		}

	protected:
		ZipIndexedFile addFile(std::string name, const std::string& contents_uncompressed, uint16_t compression_method, const std::string& contents_compressed) const;
	public:
		ZipIndexedFile addFile(std::string name, const std::string& contents) const;
		ZipIndexedFile addFileUncompressed(std::string name, const std::string& contents) const;

		void finalise(const std::vector<ZipIndexedFile>& files) const;
	};
}
