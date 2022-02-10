#pragma once

#include "base.hpp"
#if SOUP_WINDOWS

#include <cstdint>

#include <Windows.h>

#include "pointer.hpp"

namespace soup
{
	struct memory_buffer
	{
		pointer start;
		void* data;
		size_t size;

		memory_buffer() noexcept;
		explicit memory_buffer(const module& mod, pointer start, size_t size);
		~memory_buffer();

		void updateRegion(const module& mod, pointer start, size_t size);
		void release();

		[[nodiscard]] bool covers(pointer p, size_t size = 1) const noexcept;

		template <typename T>
		[[nodiscard]] T read(pointer p) const noexcept
		{
			return *reinterpret_cast<T*>(&reinterpret_cast<uint8_t*>(data)[p.as<uintptr_t>() - start.as<uintptr_t>()]);
		}
	};
}
#endif
