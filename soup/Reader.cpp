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
			__m128i e;
			if (raw(&e, 9))
			{
				const auto byte_length = 1 + bitutil::getNumTrailingZeros(~(static_cast<uint32_t>(_mm_movemask_epi8(e)) & 0xff));

				//const uint64_t mask = ((1ull << (8 * byte_length))) - 1;
				const __m128i indices = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
				__m128i limit = _mm_set1_epi8((char)byte_length);
				__m128i cmp = _mm_subs_epu8(limit, indices);
				__m128i mask = _mm_cmpeq_epi8(_mm_setzero_si128(), _mm_cmpeq_epi8(cmp, _mm_setzero_si128()));
				// mask = _mm_cmpgt_epi8(limit, indices); if using SSE4.1

				e = _mm_and_si128(e, mask);

				uint64_t lo = _pext_u64(_mm_cvtsi128_si64(e), 0x7f7f'7f7f'7f7f'7f7full);
				uint64_t hi = _mm_extract_epi64(e, 1);

				v = (hi << 56) | lo;

				seek((getPosition() - 9) + byte_length);
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
