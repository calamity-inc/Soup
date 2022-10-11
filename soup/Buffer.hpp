#pragma once

#include <cstring> // memcpy

#ifndef SOUP_BUFFER_NO_RESIZE
#define SOUP_BUFFER_NO_RESIZE false
#endif

#if SOUP_BUFFER_NO_RESIZE
#include "Exception.hpp"
#endif

namespace soup
{
	// This extremely simple implementation outperforms std::vector & std::string, barring magic optimsations, e.g. clang with std::string::push_back.
	class Buffer
	{
	public:
		uint8_t* data = nullptr;
		size_t size = 0;
		size_t capacity = 0;
#if SOUP_BUFFER_NO_RESIZE
		bool no_resize = false;
#endif

		Buffer() noexcept = default;

		Buffer(size_t capacity) noexcept
			: data(reinterpret_cast<uint8_t*>(malloc(capacity))), capacity(capacity)
		{
#if SOUP_BUFFER_NO_RESIZE
			no_resize = true;
#endif
		}

		Buffer(const Buffer& b) noexcept
			: Buffer(b.size)
		{
			append(b);
		}

		Buffer(Buffer&& b) noexcept
			: data(b.data), size(b.size), capacity(b.capacity)
		{
			b.reset();
			b.size = 0;
			b.capacity = 0;
		}

		~Buffer() noexcept
		{
			reset();
		}

		void resize(size_t desired_size) noexcept
		{
			if (size > desired_size)
			{
				size = desired_size;
			}
			resizeInner(desired_size);
		}

	private:
		void ensureSpace(size_t desired_size) noexcept
		{
			SOUP_IF_UNLIKELY (capacity < desired_size)
			{
#if SOUP_BUFFER_NO_RESIZE
				throw Exception("soup::Buffer constructed with specific size, but it did not suffice!");
#endif
				auto new_capacity = desired_size + (desired_size >> 1); // 1.5x
				resizeInner(new_capacity);
			}
		}

		void resizeInner(size_t new_capacity) noexcept
		{
			auto new_data = malloc(new_capacity);
			if (data != nullptr)
			{
				memcpy(new_data, data, size);
				free(data);
			}
			data = reinterpret_cast<uint8_t*>(new_data);
			capacity = new_capacity;
		}

	public:
		void push_back(uint8_t elm) noexcept
		{
			ensureSpace(size + 1);
			data[size++] = elm;
		}

		void emplace_back(uint8_t elm) noexcept
		{
			push_back(elm);
		}

		void append(const void* src_data, size_t src_size) noexcept
		{
			ensureSpace(size + src_size);
			memcpy(&data[size], src_data, src_size);
			size += src_size;
		}

		void append(const Buffer& src) noexcept
		{
			append(src.data, src.size);
		}

		void clear() noexcept
		{
			size = 0;
		}

	private:
		void reset() noexcept
		{
			if (data != nullptr)
			{
				free(data);
				data = nullptr;
			}
		}
	};
}
