#pragma once

#include "Reader.hpp"

namespace soup
{
	class VectorReader final : public Reader
	{
	public:
		std::vector<uint8_t> data;
		size_t offset = 0;

		VectorReader(std::vector<uint8_t> data, Endian endian = ENDIAN_LITTLE)
			: Reader(endian), data(std::move(data))
		{
		}

		~VectorReader() final = default;

		void operator =(std::vector<uint8_t> new_data) noexcept
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
			v = data.at(offset++);
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
				v.push_back(static_cast<char>(data.at(offset++)));
			}
			return true;
		}
	};
}

