#pragma once

#include "base.hpp"
#if SOUP_WINDOWS

#include <cstdint>

#include <Windows.h>

#include "Pointer.hpp"

namespace soup
{
	class MemoryBuffer
	{
	public:
		Pointer start;
		void* data;
		size_t size;

		MemoryBuffer() noexcept;
		explicit MemoryBuffer(const Module& mod, Pointer start, size_t size);
		~MemoryBuffer();

		bool updateRegion(const Module& mod, Pointer start, size_t size);
	private:
		void release();

	public:
		[[nodiscard]] bool covers(Pointer p, size_t size = 1) const noexcept;

		template <typename T>
		[[nodiscard]] T read(Pointer p) const noexcept
		{
			return *reinterpret_cast<T*>(&reinterpret_cast<uint8_t*>(data)[p.as<uintptr_t>() - start.as<uintptr_t>()]);
		}
	};
}
#endif
