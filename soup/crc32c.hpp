#pragma once

#include "base.hpp"
#if SOUP_X86

#include <cstdint>
#include <cstddef>

NAMESPACE_SOUP
{
	struct crc32c
	{
		[[nodiscard]] static uint32_t hash(const uint8_t* data, size_t size, uint32_t initial = 0) noexcept;
	};
}

#endif
