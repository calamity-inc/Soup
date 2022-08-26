#pragma once

#include "Reader.hpp"

namespace soup
{
	class StringPtrReader final : public Reader
	{
	public:
		const std::string* data;
		size_t offset = 0;

		StringPtrReader(const std::string* data, bool little_endian = true)
			: Reader(little_endian), data(data)
		{
		}

		~StringPtrReader() final = default;

		bool hasMore() final
		{
			return offset != data->size();
		}

		bool u8(uint8_t& v) final
		{
			if (offset == data->size())
			{
				return false;
			}
			v = (uint8_t)data->at(offset++);
			return true;
		}

	protected:
		bool str_impl(std::string& v, size_t len) final
		{
			if ((offset + len) > data->size())
			{
				return false;
			}
			v = data->substr(offset, len);
			offset += len;
			return true;
		}
	};
}
