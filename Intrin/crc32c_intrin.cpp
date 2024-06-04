#if defined(_MSC_VER) && !defined(__clang__) && (defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86))

#include <cstdint>

#include <nmmintrin.h>

namespace soup_intrin
{
	uint32_t crc32c_intrin(const uint8_t* data, size_t size, uint32_t initial) noexcept
	{
		uint32_t i = ~initial;
		while (size >= 8)
		{
			i = (uint32_t)_mm_crc32_u64(i, *reinterpret_cast<const uint64_t*>(data));
			data += 8;
			size -= 8;
		}
		while (size > 0)
		{
			i = _mm_crc32_u8(i, *data);
			data += 1;
			size -= 1;
		}
		return ~i;
	}
}

#endif
