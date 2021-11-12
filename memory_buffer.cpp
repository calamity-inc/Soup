#include "memory_buffer.hpp"

#include <cstdlib>

#include "module.hpp"

namespace soup
{
	memory_buffer::memory_buffer() noexcept
		: start(nullptr), data(nullptr), size(0)
	{
	}

	memory_buffer::memory_buffer(const module& mod, pointer start, size_t size)
		: start(start), data(malloc(size))
	{
		this->size = mod.externalRead(start, data, size);
	}

	memory_buffer::~memory_buffer()
	{
		release();
	}

	void memory_buffer::updateRegion(const module& mod, pointer start, size_t size)
	{
		if (this->size != size)
		{
			release();
			data = malloc(size);
		}
		this->start = start;
		this->size = mod.externalRead(start, data, size);
	}

	void memory_buffer::release()
	{
		if (data != nullptr)
		{
			free(data);
		}
	}

	bool memory_buffer::covers(pointer p, size_t size) const noexcept
	{
		return start.as<uintptr_t>() <= p.as<uintptr_t>()
			&& (p.as<uintptr_t>() + size) <= (start.as<uintptr_t>() + this->size)
			;
	}
}
