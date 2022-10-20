#pragma once

#include "fwd.hpp"

#include <vector>

#include "ZipIndexedFile.hpp"

namespace soup
{
	struct ZipReader
	{
		ioSeekableReader& is;

		ZipReader(ioSeekableReader& is);

		[[nodiscard]] size_t seekCentralDirectory() const; // returns 0 on failure

		[[nodiscard]] std::vector<ZipIndexedFile> getFileList() const;

		// Note that no decompression will be performed, the file will be returned in the way it is stored.
		// So you might want to check if uncompressed_size == compressed_size.
		[[nodiscard]] std::string getFileContents(const ZipIndexedFile& file) const;
	};
}
