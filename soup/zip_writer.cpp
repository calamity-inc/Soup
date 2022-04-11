#include "zip_writer.hpp"

#include "crc32.hpp"
#include "zip_central_directory_file.hpp"
#include "zip_end_of_central_directory.hpp"
#include "zip_local_file_header.hpp"

namespace soup
{
	zip_indexed_file zip_writer::addFile(std::string name, const std::string& contents_uncompressed, uint16_t compression_method, const std::string& contents_compressed) const
	{
		zip_indexed_file zif;
		zif.compression_method = compression_method;
		zif.uncompressed_data_crc32 = crc32::hash(contents_uncompressed);
		zif.compressed_size = contents_compressed.size();
		zif.uncompressed_size = contents_uncompressed.size();
		zif.disk_offset = os->tellp();
		zif.name = std::move(name);

		zip_local_file_header lfh{};
		lfh.common.compression_method = zif.compression_method;
		lfh.common.uncompressed_data_crc32 = zif.uncompressed_data_crc32;
		lfh.common.compressed_size = zif.compressed_size;
		lfh.common.uncompressed_size = zif.uncompressed_size;
		lfh.name = zif.name;
		os->write("\x50\x4b\x03\x04", 4);
		lfh.writeLE(*os);

		os->write(contents_compressed.data(), contents_compressed.size());

		return zif;
	}

	zip_indexed_file zip_writer::addFileUncompressed(std::string name, const std::string& contents) const
	{
		return addFile(std::move(name), contents, 0, contents);
	}

	zip_indexed_file zip_writer::addFileAnticompressed(std::string name, const std::string& contents_uncompressed) const
	{
		std::string anti_compressed{};

		for (auto i = contents_uncompressed.cbegin(); i != contents_uncompressed.cend(); ++i)
		{
			anti_compressed.push_back(i == contents_uncompressed.cend() - 1);

			anti_compressed.push_back('\x1');
			anti_compressed.push_back('\x0');

			anti_compressed.push_back('\xFE');
			anti_compressed.push_back('\xFF');

			anti_compressed.push_back(*i);
		}

		return addFile(std::move(name), contents_uncompressed, 8, anti_compressed);
	}

	void zip_writer::finalise(const std::vector<zip_indexed_file>& files) const
	{
		os->seekp(0, std::ios::end);

		zip_end_of_central_directory eocd{};
		eocd.central_directories_on_this_disk = 1;
		eocd.central_directories_in_total = 1;
		eocd.central_directory_offset = os->tellp();

		for (const auto& file : files)
		{
			zip_central_directory_file cdf{};
			cdf.common.compression_method = file.compression_method;
			cdf.common.uncompressed_data_crc32 = file.uncompressed_data_crc32;
			cdf.common.compressed_size = file.compressed_size;
			cdf.common.uncompressed_size = file.uncompressed_size;
			cdf.name = file.name;
			cdf.disk_offset = file.disk_offset;
			//cdf.external_attributes = 2;
			os->write("\x50\x4b\x01\x02", 4);
			cdf.writeLE(*os);
		}

		eocd.central_directory_size = ((uint32_t)os->tellp() - eocd.central_directory_offset);
		os->write("\x50\x4b\x05\x06", 4);
		eocd.writeLE(*os);
	}
}
