#include "ZipReader.hpp"

#include "Exception.hpp"
#include "ioSeekableReader.hpp"
#include "ZipCentralDirectoryFile.hpp"
#include "ZipEndOfCentralDirectory.hpp"
#include "ZipLocalFileHeader.hpp"

namespace soup
{
	ZipReader::ZipReader(ioSeekableReader& is)
		: is(is)
	{
		if (!is.isLittleEndian())
		{
			throw Exception("ZipReader expected underlying stream to be little endian");
		}
	}

	size_t ZipReader::seekCentralDirectory() const
	{
		std::vector<ZipIndexedFile> ret{};

		is.seekEnd();
		const size_t length = is.getPosition();
		if (length <= 22 || length == -1)
		{
			return 0;
		}
		size_t eocd_offset = (length - 22);
		for (; eocd_offset != 0; --eocd_offset)
		{
			is.seek(eocd_offset);
			std::string bytes;
			is.str(4, bytes);
			if (bytes == "\x50\x4b\x05\x06")
			{
				break;
			}
		}
		if (eocd_offset != 0)
		{
			ZipEndOfCentralDirectory eocd;
			if (!eocd.read(is))
			{
				return 0;
			}
			is.seek(eocd.central_directory_offset);
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
				std::string bytes;
				is.str(4, bytes);
				if (bytes != "\x50\x4b\x01\x02")
				{
					break;
				}
				ZipCentralDirectoryFile cdf;
				if (!cdf.read(is))
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
			} while (is.getPosition() < eocd_offset);
		}
		else
		{
			is.seek(0);
			while (true)
			{
				ZipIndexedFile zif;
				zif.disk_offset = is.getPosition();

				std::string bytes;
				is.str(4, bytes);
				if (bytes != "\x50\x4b\x03\x04")
				{
					break;
				}

				ZipLocalFileHeader lfh;
				if (!lfh.read(is))
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

		is.seek(file.disk_offset);
		std::string bytes;
		is.str(4, bytes);
		if (bytes == "\x50\x4b\x03\x04")
		{
			ZipLocalFileHeader lfh;
			if (lfh.read(is))
			{
				is.str(lfh.common.compressed_size, ret);
			}
		}

		return ret;
	}
}
