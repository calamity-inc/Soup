#include "Writer.hpp"

#if SOUP_X86 && SOUP_BITS == 64
#include <immintrin.h> // _pdep_u64

#include "CpuInfo.hpp"
#endif

NAMESPACE_SOUP
{
#if SOUP_X86 && SOUP_BITS == 64 && (defined(__GNUC__) || defined(__clang__))
	__attribute__((target("bmi2")))
#endif
	bool Writer::u64_dyn(const uint64_t& v) noexcept
	{
#if SOUP_X86 && SOUP_BITS == 64
		if (CpuInfo::get().supportsBMI2())
		{
			const auto byte_length = 1 + (v >= (1ull << 7)) + (v >= (1ull << 14)) + (v >= (1ull << 21)) + (v >= (1ull << 28)) + (v >= (1ull << 35)) + (v >= (1ull << 42)) + (v >= (1ull << 49)) + (v >= (1ull << 56));

			const uint64_t mask = ((byte_length < 9) * (1ull << (8 * (byte_length - 1)))) - 1;
			const uint64_t contbits = 0x8080'8080'8080'8080ull & mask;

			uint64_t e[2];
			e[0] = _pdep_u64(v, 0x7f7f'7f7f'7f7f'7f7full) | contbits;
			e[1] = v >> 56;

			return raw(e, byte_length);
		}
#endif
		bool ret = true;
		uint64_t in = v;
		uint8_t cur;
		for (uint8_t i = 0; i != 8; ++i)
		{
			cur = (in & 0x7f);
			in >>= 7;
			if (in != 0)
			{
				cur |= 0x80;
				ret &= u8(cur);
			}
			else
			{
				ret &= u8(cur);
				return ret;
			}
		}
		cur = (uint8_t)in;
		ret &= u8(cur);
		return ret;
	}

	bool Writer::i64_dyn(const int64_t& v) noexcept
	{
		uint64_t u;
		bool neg = (v < 0);
		if (neg)
		{
			u = (~v + 1) & ~((uint64_t)1 << 63);
		}
		else
		{
			u = v;
		}
		return u64_dyn(((uint64_t)neg << 6) | ((u & ~0x3f) << 1) | (u & 0x3f));
	}

#if SOUP_X86 && SOUP_BITS == 64 && (defined(__GNUC__) || defined(__clang__))
	__attribute__((target("bmi2")))
#endif
	bool Writer::u64_dyn_v2(const uint64_t& v) noexcept
	{
#if SOUP_X86 && SOUP_BITS == 64
		if (CpuInfo::get().supportsBMI2())
		{
			const auto byte_length = 1 + (v >= (1ull << 7)) + (v >= (1ull << 14)) + (v >= (1ull << 21)) + (v >= (1ull << 28)) + (v >= (1ull << 35)) + (v >= (1ull << 42)) + (v >= (1ull << 49)) + (v >= (1ull << 56));

			const uint64_t mask = ((byte_length < 9) * (1ull << (8 * (byte_length - 1)))) - 1;
			const uint64_t contbits = 0x8080'8080'8080'8080ull & mask;

			// v2
			const auto subbits = (byte_length >= 2) * (byte_length - 1);
			const auto submask = ((1u << subbits) - 1u);
			uint64_t w = v - (_pdep_u64(submask, 0x0002040810204081ull) << 7);

			uint64_t e[2];
			e[0] = _pdep_u64(w, 0x7f7f'7f7f'7f7f'7f7full) | contbits;
			e[1] = w >> 56;

			return raw(e, byte_length);
		}
#endif
		bool ret = true;
		uint64_t in = v;
		uint8_t cur;
		for (uint8_t i = 0; i != 8; ++i)
		{
			cur = (in & 0x7f);
			in >>= 7;
			if (in != 0)
			{
				cur |= 0x80;
				ret &= u8(cur);
				in -= 1; // v2
			}
			else
			{
				ret &= u8(cur);
				return ret;
			}
		}
		cur = (uint8_t)in;
		ret &= u8(cur);
		return ret;
	}

	bool Writer::i64_dyn_v2(const int64_t& v) noexcept
	{
		uint64_t u;
		bool neg = (v < 0);
		if (neg)
		{
			u = ~v;
		}
		else
		{
			u = v;
		}
		return u64_dyn_v2(((uint64_t)neg << 6) | ((u & ~0x3f) << 1) | (u & 0x3f));
	}

	bool Writer::mysql_lenenc(const uint64_t& v) noexcept
	{
		if (v < 0xFB)
		{
			auto val = (uint8_t)v;
			return u8(val);
		}
		else if (v <= 0xFFFF)
		{
			uint8_t prefix = 0xFC;
			auto val = (uint16_t)v;
			return u8(prefix) && u16_le(val);
		}
		else if (v <= 0xFFFFFF)
		{
			uint8_t prefix = 0xFD;
			auto val = (uint32_t)v;
			return u8(prefix) && u24_le(val);
		}
		uint8_t prefix = 0xFE;
		auto val = v;
		return u8(prefix) && u64_le(val);
	}
}
