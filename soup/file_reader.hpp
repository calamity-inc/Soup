#pragma once

#include "reader.hpp"

#include <filesystem>
#include <fstream>

namespace soup
{
	class file_reader final : public reader
	{
	public:
		std::ifstream s;

		file_reader(const std::string& path, bool little_endian = true)
			: reader(little_endian), s(path)
		{
		}

		template <class T, std::enable_if_t<std::is_same_v<T, std::filesystem::path>, int> = 0>
		file_reader(const T& path, bool little_endian = true)
			: reader(little_endian), s(path)
		{
		}

		~file_reader() final = default;

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
