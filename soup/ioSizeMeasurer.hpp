#pragma once

#include "Writer.hpp"

NAMESPACE_SOUP
{
	class ioSizeMeasurer final : public Writer
	{
	public:
		std::streamsize size = 0;

		ioSizeMeasurer()
			: Writer()
		{
		}

		~ioSizeMeasurer() final = default;

		bool raw(void* data, size_t size) noexcept final
		{
			this->size += size;
			return true;
		}

		[[nodiscard]] std::streamoff getPosition() final
		{
			return size;
		}
	};
}
