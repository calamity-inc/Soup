#include "../soup/base.hpp"
#if defined(_MSC_VER) && !defined(__clang__) && SOUP_X86

#include <cstdint>

#include <nmmintrin.h>

NAMESPACE_SOUP
{
	namespace intrin
	{
		uint32_t crc32c_intrin(const uint8_t* data, size_t size, uint32_t initial) noexcept
		{
			uint32_t i = ~initial;
#if SOUP_BITS == 64
			while (size >= 8)
			{
				i = (uint32_t)_mm_crc32_u64(i, *reinterpret_cast<const uint64_t*>(data));
				data += 8;
				size -= 8;
			}
#else
			while (size >= 4)
			{
				i = _mm_crc32_u32(i, *reinterpret_cast<const uint32_t*>(data));
				data += 4;
				size -= 4;
			}
#endif
			while (size > 0)
			{
				i = _mm_crc32_u8(i, *data);
				data += 1;
				size -= 1;
			}
			return ~i;
		}
	}
}

#endif
