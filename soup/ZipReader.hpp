#pragma once

#include <istream>
#include <vector>

#include "ZipIndexedFile.hpp"

namespace soup
{
	struct ZipReader
	{
		std::istream* is;

		ZipReader(std::istream* is)
			: is(is)
		{
		}

		[[nodiscard]] size_t seekCentralDirectory() const;

		[[nodiscard]] std::vector<ZipIndexedFile> getFileList() const;

		// Note that no decompression will be performed, the file will be returned in the way it is stored.
		// So you might want to check if uncompressed_size == compressed_size.
		[[nodiscard]] std::string getFileContents(const ZipIndexedFile& file) const;
	};
}
