#include "MemoryBuffer.hpp"

#if SOUP_WINDOWS

#include <cstdlib>

#include "Module.hpp"

namespace soup
{
	MemoryBuffer::MemoryBuffer() noexcept
		: start(nullptr), data(nullptr), size(0)
	{
	}

	MemoryBuffer::MemoryBuffer(const Module& mod, Pointer start, size_t size)
		: start(start), data(malloc(size))
	{
		this->size = mod.externalRead(start, data, size);
	}

	MemoryBuffer::~MemoryBuffer()
	{
		release();
	}

	void MemoryBuffer::updateRegion(const Module& mod, Pointer start, size_t size)
	{
		if (this->size != size)
		{
			release();
			data = malloc(size);
		}
		this->start = start;
		this->size = mod.externalRead(start, data, size);
	}

	void MemoryBuffer::release()
	{
		if (data != nullptr)
		{
			free(data);
		}
	}

	bool MemoryBuffer::covers(Pointer p, size_t size) const noexcept
	{
		return start.as<uintptr_t>() <= p.as<uintptr_t>()
			&& (p.as<uintptr_t>() + size) <= (start.as<uintptr_t>() + this->size)
			;
	}
}

#endif
