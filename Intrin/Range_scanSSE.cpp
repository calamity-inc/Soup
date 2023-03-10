#include <Range.hpp>

#include <intrin.h>

#include <Pattern.hpp>

namespace soup
{
	Pointer Range::scanSSE(const Pattern& sig) const noexcept
	{
		auto data = sig.bytes.data();
		auto length = sig.bytes.size();
		const auto match = _mm_set1_epi8(data[0].value());
		for (uintptr_t i = 0; i < size - length; i += 16)
		{
			int mask = _mm_movemask_epi8(_mm_cmpeq_epi8(match, _mm_load_si128(base.add(i).as<__m128i*>())));
			if (mask != 0)
			{
				for (uintptr_t j = 0; j != 16; ++j)
				{
					if ((mask >> j) & 1)
					{
						//if (pattern_matches(base.add(i).add(j).as<uint8_t*>(), data, length))
						if (pattern_matches(base.add(i).add(j).add(1).as<uint8_t*>(), data + 1, length - 1))
						{
							return base.add(i).add(j);
						}
					}
				}
			}

		}
		return Pointer();
	}
}
