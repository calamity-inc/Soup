#pragma once

#include "Reader.hpp"

namespace soup
{
	class StringReader final : public Reader
	{
	public:
		std::string data;
		size_t offset = 0;

		StringReader(std::string data, bool little_endian = true)
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
			if (offset + len > data.size())
			{
				return false;
			}
			v = data.substr(offset, len);
			offset += len;
			return true;
		}
	};
}
