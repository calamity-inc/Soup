#include "Range.hpp"

#include "base.hpp"

#if SOUP_X86 && SOUP_BITS == 64
#include <emmintrin.h>
#endif

#if SOUP_WINDOWS
#include <Windows.h>
#endif

#include "bitutil.hpp"
#include "CpuInfo.hpp"
#include "Pattern.hpp"

NAMESPACE_SOUP
{
	Range::Range(Pointer base, size_t size) noexcept
		: base(base), size(size)
	{
	}

	Pointer Range::end() const noexcept
	{
		return base.add(size);
	}

	bool Range::contains(Pointer h) const noexcept
	{
		return h.as<uintptr_t>() >= base.as<uintptr_t>() && h.as<uintptr_t>() <= end().as<uintptr_t>();
	}

	bool Range::pattern_matches(uint8_t* target, const std::optional<uint8_t>* sig, size_t length) noexcept
	{
#if SOUP_WINDOWS
		__try
		{
#endif
			for (size_t i = 0; i < length; ++i)
			{
				if (sig[i] && *sig[i] != target[i])
				{
					return false;
				}
			}
			return true;
#if SOUP_WINDOWS
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
		return false;
#endif
	}

	Pointer Range::scan(const Pattern& sig) const noexcept
	{
		Pointer ptr{};
		SOUP_UNUSED(scanWithMultipleResults(sig, &ptr, 1));
		return ptr;
	}

	size_t Range::scanWithMultipleResults(const Pattern& sig, Pointer buf[], size_t buflen) const noexcept
	{
		const auto data = sig.bytes.data();
		const auto length = sig.bytes.size();
		SOUP_IF_UNLIKELY (length == 0)
		{
			return 0;
		}
#if SOUP_X86 && SOUP_BITS == 64
		SOUP_IF_LIKELY (data[0].has_value())
		{
			if (CpuInfo::get().supportsSSE2())
			{
				return scanWithMultipleResultsSimd(sig, buf, buflen);
			}
		}
#endif
		size_t accum = 0;
		for (uintptr_t i = 0; i != (size - length); ++i)
		{
			if (pattern_matches(base.add(i).as<uint8_t*>(), data, length))
			{
				buf[accum++] = base.add(i);
				if (accum == buflen)
				{
					break;
				}
			}
		}
		return accum;
	}

#if SOUP_X86 && SOUP_BITS == 64
	size_t Range::scanWithMultipleResultsSimd(const Pattern& sig, Pointer buf[], size_t buflen) const noexcept
	{
		const auto data = sig.bytes.data();
		const auto length = sig.bytes.size();
		const auto match = _mm_set1_epi8(*data[0]);
		size_t accum = 0;
		for (uintptr_t i = 0; i < (size - length); i += 16)
		{
			int mask = _mm_movemask_epi8(_mm_cmpeq_epi8(match, _mm_loadu_si128(base.add(i).as<__m128i*>())));
			while (mask != 0)
			{
				const auto j = bitutil::getLeastSignificantSetBit(mask);
				if (pattern_matches(base.add(i).add(j).add(1).as<uint8_t*>(), data + 1, length - 1))
				{
					buf[accum++] = base.add(i).add(j);
					if (accum == buflen)
					{
						return buflen;
					}
				}
				bitutil::unsetLeastSignificantSetBit(mask);
			}
		}
		return accum;
	}
#endif
}
