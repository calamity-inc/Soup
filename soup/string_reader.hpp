#pragma once

#include "reader.hpp"

namespace soup
{
	class string_reader final : public reader
	{
	public:
		std::string data;
		size_t offset = 0;

		string_reader(std::string data, bool little_endian = true)
			: reader(little_endian), data(std::move(data))
		{
		}

		~string_reader() final = default;

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
