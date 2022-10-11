#pragma once

#include "Reader.hpp"

namespace soup
{
	class StringReader final : public Reader
	{
	public:
		std::string data;
		size_t offset = 0;

		StringReader(Endian endian = LITTLE_ENDIAN) noexcept
			: Reader(endian)
		{
		}

		StringReader(std::string data, Endian endian = LITTLE_ENDIAN)
			: Reader(endian), data(std::move(data))
		{
		}
		
		StringReader(std::string data, bool little_endian)
			: Reader(little_endian), data(std::move(data))
		{
		}

		~StringReader() final = default;

		void operator =(std::string new_data) noexcept
		{
			data = std::move(new_data);
			offset = 0;
		}

		bool hasMore() final
		{
			return offset != data.size();
		}

		bool u8(uint8_t& v) final
		{
			if (offset == data.size())
			{
				return false;
			}
			v = (uint8_t)data.at(offset++);
			return true;
		}

	protected:
		bool str_impl(std::string& v, size_t len) final
		{
			if ((offset + len) > data.size())
			{
				return false;
			}
			v = data.substr(offset, len);
			offset += len;
			return true;
		}

	public:
		// Faster alternative to std::stringstream + std::getline
		bool getLine(std::string& line) noexcept
		{
			size_t next = data.find('\n', offset);
			SOUP_IF_LIKELY (next != std::string::npos)
			{
				line = data.substr(offset, next - offset);
				offset = next + 1;
				return true;
			}
			// We're on the last line
			if (offset != data.size()) // Not the last byte?
			{
				line = data.substr(offset);
				offset = data.size();
				return true;
			}
			return false;
		}
	};
}
