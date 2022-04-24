#include "ZipReader.hpp"

#include <cstring> // memcmp

#include "ZipCentralDirectoryFile.hpp"
#include "ZipEndOfCentralDirectory.hpp"
#include "ZipLocalFileHeader.hpp"

namespace soup
{
	size_t ZipReader::seekCentralDirectory() const
	{
		std::vector<ZipIndexedFile> ret{};

		is->seekg(0, std::ios::end);
		const size_t length = is->tellg();
		if (length <= 22 || length == -1)
		{
			return 0;
		}
		size_t eocd_offset = (length - 22);
		for (; eocd_offset != 0; --eocd_offset)
		{
			is->seekg(eocd_offset);
			char bytes[4];
			is->read(bytes, 4);
			if (memcmp(bytes, "\x50\x4b\x05\x06", 4) == 0)
			{
				break;
			}
		}
		if (eocd_offset != 0)
		{
			ZipEndOfCentralDirectory eocd;
			if (!eocd.readLE(*is))
			{
				return 0;
			}
			is->seekg(eocd.central_directory_offset);
		}

		return eocd_offset;
	}

	std::vector<ZipIndexedFile> ZipReader::getFileList() const
	{
		std::vector<ZipIndexedFile> ret{};

		if (auto eocd_offset = seekCentralDirectory())
		{
			do
			{
				char bytes[4];
				is->read(bytes, 4);
				if (memcmp(bytes, "\x50\x4b\x01\x02", 4) != 0)
				{
					break;
				}
				ZipCentralDirectoryFile cdf;
				if (!cdf.readLE(*is))
				{
					break;
				}
				ret.emplace_back(ZipIndexedFile{
					cdf.common.compression_method,
					cdf.common.uncompressed_data_crc32,
					cdf.common.compressed_size,
					cdf.common.uncompressed_size,
					std::move(cdf.name),
					cdf.disk_offset,
				});
			} while (is->tellg() < eocd_offset);
		}
		else
		{
			is->seekg(0);
			while (true)
			{
				ZipIndexedFile zif;
				zif.disk_offset = is->tellg();

				char bytes[4] = { 0 };
				is->read(bytes, 4);
				if (memcmp(bytes, "\x50\x4b\x03\x04", 4) != 0)
				{
					break;
				}

				ZipLocalFileHeader lfh;
				if (!lfh.readLE(*is))
				{
					break;
				}

				zif.compression_method = lfh.common.compression_method;
				zif.uncompressed_data_crc32 = lfh.common.uncompressed_data_crc32;
				zif.compressed_size = lfh.common.compressed_size;
				zif.uncompressed_size = lfh.common.uncompressed_size;
				zif.name = std::move(lfh.name);

				ret.emplace_back(std::move(zif));
			}
		}

		return ret;
	}

	std::string ZipReader::getFileContents(const ZipIndexedFile& file) const
	{
		std::string ret{};

		is->clear();
		is->seekg(file.disk_offset);
		char bytes[4];
		is->read(bytes, 4);
		if (memcmp(bytes, "\x50\x4b\x03\x04", 4) == 0)
		{
			ZipLocalFileHeader lfh;
			if (lfh.readLE(*is))
			{
				ret = std::string(lfh.common.compressed_size, 0);
				is->read(ret.data(), lfh.common.compressed_size);
			}
		}

		return ret;
	}
}
