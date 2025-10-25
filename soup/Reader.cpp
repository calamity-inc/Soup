#include "Reader.hpp"

#if SOUP_X86 && SOUP_BITS == 64
#include <emmintrin.h> // _mm_movemask_epi8
#include <immintrin.h> // _pext_u64

#include "bitutil.hpp"
#include "CpuInfo.hpp"
#endif

NAMESPACE_SOUP
{
#if SOUP_X86 && SOUP_BITS == 64 && (defined(__GNUC__) || defined(__clang__))
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
				const uint32_t contbits = _mm_movemask_epi8(e) & 0xff;
				const auto byte_length = 1 + bitutil::getNumTrailingZeros(~contbits);

				const uint64_t mask = ((byte_length < 8) * (1ull << (8 * byte_length))) - 1;
				uint64_t lo = _pext_u64(_mm_cvtsi128_si64(e) & mask, 0x7f7f'7f7f'7f7f'7f7full);
				uint64_t hi = _mm_extract_epi64(e, 1) * (byte_length == 9);
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

#if SOUP_X86 && SOUP_BITS == 64 && (defined(__GNUC__) || defined(__clang__))
	__attribute__((target("bmi2")))
#endif
	bool Reader::u64_dyn_v2(uint64_t& v) noexcept
	{
		v = 0;
#if SOUP_X86 && SOUP_BITS == 64
		if (CpuInfo::get().supportsSSE2() && CpuInfo::get().supportsBMI2())
		{
			__m128i e;
			if (raw(&e, 9))
			{
				const uint32_t contbits = _mm_movemask_epi8(e) & 0xff;
				const auto byte_length = 1 + bitutil::getNumTrailingZeros(~contbits);

				const uint64_t mask = ((byte_length < 8) * (1ull << (8 * byte_length))) - 1;
				uint64_t lo = _pext_u64(_mm_cvtsi128_si64(e) & mask, 0x7f7f'7f7f'7f7f'7f7full);
				uint64_t hi = _mm_extract_epi64(e, 1) * (byte_length == 9);
				v = (hi << 56) | lo;

				// v2
				const auto addbits = (byte_length >= 2) * (byte_length - 1);
				const auto addmask = ((1u << addbits) - 1u);
				v += _pdep_u64(addmask, 0x0002040810204081ull) << 7;

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
