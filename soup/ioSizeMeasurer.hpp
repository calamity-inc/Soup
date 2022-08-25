#pragma once

#include "Writer.hpp"

namespace soup
{
	class ioSizeMeasurer final : public Writer
	{
	public:
		size_t size;

		ioSizeMeasurer()
			: Writer(SOUP_LITTLE_ENDIAN)
		{
		}

		~ioSizeMeasurer() final = default;

	protected:
		void write(const char* data, size_t size) final
		{
			this->size += size;
		}
	};
}
