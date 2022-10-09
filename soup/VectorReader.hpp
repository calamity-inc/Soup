#pragma once

#include "Reader.hpp"

namespace soup
{
	class VectorReader final : public Reader
	{
	public:
		std::vector<char> data;
		size_t offset = 0;

		VectorReader(std::vector<char> data, Endian endian = LITTLE_ENDIAN)
			: Reader(endian), data(std::move(data))
		{
		}

		~VectorReader() final = default;

		void operator =(std::vector<char> new_data) noexcept
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
			while (len--)
			{
				v.push_back(data.at(offset++));
			}
			return true;
		}
	};
}

