#include "Reader.hpp"

#if SOUP_X86 && SOUP_BITS == 64
#include <emmintrin.h> // _mm_movemask_epi8
#include <immintrin.h> // _pext_u64

#include "bitutil.hpp"
#include "CpuInfo.hpp"
#endif

NAMESPACE_SOUP
{
#if defined(__GNUC__) || defined(__clang__)
	__attribute__((target("bmi2")))
#endif
	bool Reader::u64_dyn(uint64_t& v) noexcept
	{
		v = 0;
#if SOUP_X86 && SOUP_BITS == 64
		if (CpuInfo::get().supportsSSE2() && CpuInfo::get().supportsBMI2())
		{
			uint64_t x;
			if (raw(&x, sizeof(x)))
			{
				const auto pos = getPosition() - 8;
				const auto byte_length = 1 + bitutil::getNumTrailingZeros(~(uint32_t)_mm_movemask_epi8(_mm_loadl_epi64((const __m128i*) & x)));
				const uint64_t mask = ((byte_length < 8) * (1ull << (8 * byte_length))) - 1;
				x = _pext_u64(x, 0x7f7f'7f7f'7f7f'7f7full & mask);
				if (byte_length == 9)
				{
					uint64_t y;
					SOUP_RETHROW_FALSE(raw(&y, 1));
					x |= y << 56;
				}
				v = x;
				seek(pos + byte_length);
				return true;
			}
		}
#endif
		uint8_t b;
		uint8_t bits = 0;
		for (uint8_t i = 0; i != 8; ++i)
		{
			SOUP_RETHROW_FALSE(u8(b));
			v += (uint64_t)(b & 0x7f) << bits;
			if (!(b >> 7))
			{
				return true;
			}
			bits += 7;
		}
		SOUP_RETHROW_FALSE(u8(b));
		v += (uint64_t)b << 56;
		return true;
	}

	bool Reader::i64_dyn(int64_t& v) noexcept
	{
		uint64_t u;
		SOUP_RETHROW_FALSE(u64_dyn(u));
		const bool neg = (u >> 6) & 1; // check bit 6
		v = ((u >> 1) & ~0x3f) | (u & 0x3f); // remove bit 6
		if (neg)
		{
			v = static_cast<int64_t>(~(v - 1) | (static_cast<uint64_t>(1) << 63));
		}
		return true;
	}

	bool Reader::u64_dyn_v2(uint64_t& v) noexcept
	{
		v = 0;
		uint8_t b;
		uint8_t bits = 0;
		for (uint8_t i = 0; i != 8; ++i)
		{
			SOUP_RETHROW_FALSE(u8(b));
			v += (uint64_t)(b & 0x7f) << bits;
			if (!(b >> 7))
			{
				return true;
			}
			bits += 7;
			v += (uint64_t)1 << bits; // v2
		}
		SOUP_RETHROW_FALSE(u8(b));
		v += (uint64_t)b << 56;
		return true;
	}

	bool Reader::i64_dyn_v2(int64_t& v) noexcept
	{
		uint64_t u;
		SOUP_RETHROW_FALSE(u64_dyn_v2(u));
		const bool neg = (u >> 6) & 1; // check bit 6
		u = ((u >> 1) & ~0x3f) | (u & 0x3f); // remove bit 6
		if (neg)
		{
			v = ~u;
		}
		else
		{
			v = u;
		}
		return true;
	}
}
