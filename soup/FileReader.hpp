#pragma once

#include "Reader.hpp"

#include <filesystem>
#include <fstream>

#include "type_traits.hpp"

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

#if SOUP_WINDOWS
		FileReader(const std::wstring& path, bool little_endian = true)
			: Reader(little_endian), s(path, std::ios::binary)
		{
		}
#endif

		template <class T, SOUP_RESTRICT(std::is_same_v<T, std::filesystem::path>)>
		FileReader(const T& path, bool little_endian = true)
			: Reader(little_endian), s(path, std::ios::binary)
		{
		}

		~FileReader() final = default;

		bool hasMore() noexcept final
		{
#if SOUP_EXCEPTIONS
			try
#endif
			{
				return s.peek() != EOF;
			}
#if SOUP_EXCEPTIONS
			catch (...)
			{
			}
			return false;
#endif
		}

		bool raw(void* data, size_t len) noexcept final
		{
#if SOUP_EXCEPTIONS
			try
#endif
			{
				s.read(reinterpret_cast<char*>(data), len);
			}
#if SOUP_EXCEPTIONS
			catch (...)
			{
				return false;
			}
#endif
			return s.rdstate() == 0;
		}

		bool getLine(std::string& line) noexcept final
		{
			if (Reader::getLine(line))
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
