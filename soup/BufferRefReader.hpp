#pragma once

#include "ioSeekableReader.hpp"

namespace soup
{
	class BufferRefReader final : public ioSeekableReader
	{
	public:
		const Buffer& data;
		size_t offset = 0;

		BufferRefReader(const Buffer& data, bool little_endian = true)
			: ioSeekableReader(little_endian), data(data)
		{
		}

		~BufferRefReader() final = default;

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
			//v = data.substr(offset, len);
			v = std::string((const char*)data.data() + offset, len);
			offset += len;
			return true;
		}

	public:
		[[nodiscard]] size_t getPosition() final
		{
			return offset;
		}

		void seek(size_t pos) final
		{
			offset = pos;
		}

		void seekEnd() final
		{
			offset = data.size();
		}
	};
}
