#pragma once

#include "Reader.hpp"

#include <filesystem>
#include <fstream>

namespace soup
{
	class FileReader final : public Reader
	{
	public:
		std::ifstream s;

		FileReader(const std::string& path, bool little_endian = true)
			: Reader(little_endian), s(path, std::ios::binary)
		{
		}

		template <class T, SOUP_RESTRICT(std::is_same_v<T, std::filesystem::path>)>
		FileReader(const T& path, bool little_endian = true)
			: Reader(little_endian), s(path, std::ios::binary)
		{
		}

		~FileReader() final = default;

		bool hasMore() final
		{
			return s.peek() != EOF;
		}

		bool u8(uint8_t& v) final
		{
			return !s.read((char*)&v, sizeof(uint8_t)).bad();
		}

	protected:
		bool str_impl(std::string& v, size_t len) final
		{
			v = std::string(len, 0);
			s.read(v.data(), len);
			return !s.bad();
		}
	};
}
