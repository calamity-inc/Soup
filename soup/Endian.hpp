#pragma once

#include "base.hpp"

#if SOUP_CPP20
#include <bit>
#endif

#include "IntStruct.hpp"

#undef LITTLE_ENDIAN
#undef BIG_ENDIAN

namespace soup
{
	enum Endian : bool
	{
		LITTLE_ENDIAN = true,
		BIG_ENDIAN = false,
#if SOUP_CPP20
		NATIVE_ENDIAN = (std::endian::native == std::endian::little),
#else
		NATIVE_ENDIAN = true,
#endif
		NETWORK_ENDIAN = BIG_ENDIAN,
	};

	SOUP_INT_STRUCT(native_u16_t, uint16_t);
	SOUP_INT_STRUCT(native_u32_t, uint32_t);
	SOUP_INT_STRUCT(network_u16_t, uint16_t);
	SOUP_INT_STRUCT(network_u32_t, uint32_t);

	struct Endianness
	{
		// C++23 will add std::byteswap

		[[nodiscard]] static constexpr uint16_t invert(uint16_t val)
		{
#if defined(__GNUC__) || defined(__clang__)
			return __builtin_bswap16(val);
#else
			return (val << 8) | (val >> 8);
#endif
		}
		
		[[nodiscard]] static constexpr uint32_t invert(uint32_t val)
		{
#if defined(__GNUC__) || defined(__clang__)
			return __builtin_bswap32(val);
#else
			return val << (32 - 8)
				| ((val >> 8) & 0xFF) << (32 - 16)
				| ((val >> 16) & 0xFF) << (32 - 24)
				| ((val >> 24) & 0xFF)
				;
#endif
		}

		[[nodiscard]] static constexpr uint64_t invert(uint64_t val)
		{
#if defined(__GNUC__) || defined(__clang__)
			return __builtin_bswap64(val);
#else
			return val << (64 - 8)
				| ((val >> 8) & 0xFF) << (64 - 16)
				| ((val >> 16) & 0xFF) << (64 - 24)
				| ((val >> 24) & 0xFF) << (64 - 32)
				| ((val >> 32) & 0xFF) << (64 - 40)
				| ((val >> 40) & 0xFF) << (64 - 48)
				| ((val >> 48) & 0xFF) << (64 - 56)
				| ((val >> 56) & 0xFF)
				;
#endif
		}

		[[nodiscard]] static network_u16_t toNetwork(native_u16_t val)
		{
			if constexpr (NATIVE_ENDIAN == LITTLE_ENDIAN)
			{
				return invert(val);
			}
			return network_u16_t(val);
		}

		[[nodiscard]] static network_u32_t toNetwork(native_u32_t val)
		{
			if constexpr (NATIVE_ENDIAN == LITTLE_ENDIAN)
			{
				return invert(val);
			}
			return network_u32_t(val);
		}

		[[nodiscard]] static native_u16_t toNative(network_u16_t val)
		{
			if constexpr (NATIVE_ENDIAN == LITTLE_ENDIAN)
			{
				return invert(val);
			}
			return native_u16_t(val);
		}

		[[nodiscard]] static native_u32_t toNative(network_u32_t val)
		{
			if constexpr (NATIVE_ENDIAN == LITTLE_ENDIAN)
			{
				return invert(val);
			}
			return native_u32_t(val);
		}
	};
}
