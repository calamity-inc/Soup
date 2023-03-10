#include <Range.hpp>

#include <intrin.h>

#include <bitutil.hpp>
#include <Pattern.hpp>

namespace soup
{
	Pointer Range::scanSSE(const Pattern& sig) const noexcept
	{
		auto data = sig.bytes.data();
		auto length = sig.bytes.size();
		const auto match = _mm_set1_epi8(data[0].value());
		for (uintptr_t i = 0; i < size - length; )
		{
			int mask = _mm_movemask_epi8(_mm_cmpeq_epi8(match, _mm_loadu_si128(base.add(i).as<__m128i*>())));
			if (mask == 0)
			{
				i += 16;
				continue;
			}
			auto j = bitutil::getLeastSignificantSetBit(mask);
			if (pattern_matches(base.add(i).add(j).add(1).as<uint8_t*>(), data + 1, length - 1))
			{
				return base.add(i).add(j);
			}
			i += (j + 1);
		}
		return nullptr;
	}
}
