#pragma once

#include "ioSeekableReader.hpp"

#include <filesystem>
#include <fstream>

#include "type_traits.hpp"

namespace soup
{
	class FileReader final : public ioSeekableReader
	{
	public:
		std::ifstream s;

		FileReader(const std::string& path, bool little_endian = true)
			: ioSeekableReader(little_endian), s(path, std::ios::binary)
		{
		}

#if SOUP_WINDOWS
		FileReader(const std::wstring& path, bool little_endian = true)
			: ioSeekableReader(little_endian), s(path, std::ios::binary)
		{
		}
#endif

		template <class T, SOUP_RESTRICT(std::is_same_v<T, std::filesystem::path>)>
		FileReader(const T& path, bool little_endian = true)
			: ioSeekableReader(little_endian), s(path, std::ios::binary)
		{
		}

		~FileReader() final = default;

		bool hasMore() final
		{
			return s.peek() != EOF;
		}

		bool u8(uint8_t& v) final
		{
			s.read((char*)&v, sizeof(uint8_t));
			return !s.bad() && !s.eof() && s.is_open();
		}

	protected:
		bool str_impl(std::string& v, size_t len) final
		{
			v = std::string(len, 0);
			s.read(v.data(), len);
			return !s.bad();
		}

	public:
		bool getLine(std::string& line) noexcept final
		{
			if (ioSeekableReader::getLine(line))
			{
				if (!line.empty()
					&& line.back() == '\r'
					)
				{
					line.pop_back();
				}
				return true;
			}
			return false;
		}

		[[nodiscard]] size_t getPosition() final
		{
			return static_cast<size_t>(s.tellg());
		}

		void seek(size_t pos) final
		{
			s.seekg(pos);
			s.clear();
		}

		void seekEnd() final
		{
			s.seekg(0, std::ios::end);
		}
	};
}
