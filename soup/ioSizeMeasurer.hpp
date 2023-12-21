#pragma once

#include "Writer.hpp"

namespace soup
{
	class ioSizeMeasurer final : public Writer
	{
	public:
		size_t size = 0;

		ioSizeMeasurer()
			: Writer(ENDIAN_NATIVE)
		{
		}

		~ioSizeMeasurer() final = default;

		void write(const char* data, size_t size) final
		{
			this->size += size;
		}
	};
}
