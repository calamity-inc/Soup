#include "zip_reader.hpp"

#include "zip_central_directory_file.hpp"
#include "zip_end_of_central_directory.hpp"
#include "zip_local_file_header.hpp"

namespace soup
{
	size_t zip_reader::seekCentralDirectory() const
	{
		std::vector<zip_indexed_file> ret{};

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
			zip_end_of_central_directory eocd;
			if (!eocd.readLE(*is))
			{
				return 0;
			}
			is->seekg(eocd.central_directory_offset);
		}

		return eocd_offset;
	}

	std::vector<zip_indexed_file> zip_reader::getFileList() const
	{
		std::vector<zip_indexed_file> ret{};

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
				zip_central_directory_file cdf;
				if (!cdf.readLE(*is))
				{
					break;
				}
				ret.emplace_back(zip_indexed_file{
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
				zip_indexed_file zif;
				zif.disk_offset = is->tellg();

				char bytes[4] = { 0 };
				is->read(bytes, 4);
				if (memcmp(bytes, "\x50\x4b\x03\x04", 4) != 0)
				{
					break;
				}

				zip_local_file_header lfh;
				if (!lfh.readLE(*is))
				{
					break;
				}

				zif.uncompressed_data_crc32 = lfh.common.uncompressed_data_crc32;
				zif.compressed_size = lfh.common.compressed_size;
				zif.uncompressed_size = lfh.common.uncompressed_size;
				zif.name = std::move(lfh.name);

				ret.emplace_back(std::move(zif));
			}
		}

		return ret;
	}

	std::string zip_reader::getFileContents(const zip_indexed_file& file) const
	{
		std::string ret{};

		is->clear();
		is->seekg(file.disk_offset);
		char bytes[4];
		is->read(bytes, 4);
		if (memcmp(bytes, "\x50\x4b\x03\x04", 4) == 0)
		{
			zip_local_file_header lfh;
			if (lfh.readLE(*is))
			{
				ret = std::string(lfh.common.compressed_size, 0);
				is->read(ret.data(), lfh.common.compressed_size);
			}
		}

		return ret;
	}
}
