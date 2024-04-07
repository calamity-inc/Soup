#pragma once

#include "Reader.hpp"

#include <cstring> // memcpy

NAMESPACE_SOUP
{
	class BufferRefReader final : public Reader
	{
	public:
		const Buffer& data;
		size_t offset = 0;

		BufferRefReader(const Buffer& data, bool little_endian = true)
			: Reader(little_endian), data(data)
		{
		}

		~BufferRefReader() final = default;

		bool hasMore() noexcept final
		{
			return offset != data.size();
		}

		bool raw(void* data, size_t len) noexcept final
		{
			SOUP_IF_UNLIKELY ((offset + len) > this->data.size())
			{
				return false;
			}
			memcpy(reinterpret_cast<char*>(data), this->data.data() + offset, len);
			offset += len;
			return true;
		}

		[[nodiscard]] size_t getPosition() noexcept final
		{
			return offset;
		}

		void seek(size_t pos) noexcept final
		{
			offset = pos;
		}

		void seekEnd() noexcept final
		{
			offset = data.size();
		}
	};
}
