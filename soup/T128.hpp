#pragma once

namespace soup
{
	template <typename T>
	struct T128
	{
		__m128i data;

		explicit SOUP_FORCEINLINE T128(T val)
		{
			if constexpr (sizeof(T) == 1)
			{
				data = _mm_set1_epi8(val);
			}
			else if constexpr (sizeof(T) == 2)
			{
				data = _mm_set1_epi16(val);
			}
			else if constexpr (sizeof(T) == 4)
			{
				data = _mm_set1_epi32(val);
			}
			else if constexpr (sizeof(T) == 8)
			{
				data = _mm_set1_epi64x(val);
			}
		}

		[[nodiscard]] SOUP_FORCEINLINE __m128i operator==(const __m128i& b) const noexcept
		{
			if constexpr (sizeof(T) == 1)
			{
				return _mm_cmpeq_epi8(data, b);
			}
			else if constexpr (sizeof(T) == 2)
			{
				return _mm_cmpeq_epi16(data, b);
			}
			else if constexpr (sizeof(T) == 4)
			{
				return _mm_cmpeq_epi32(data, b);
			}
			else if constexpr (sizeof(T) == 8)
			{
				return _mm_cmpeq_epi64(data, b);
			}
		}
	};
}
