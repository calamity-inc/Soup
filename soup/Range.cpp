#include "Range.hpp"

#if SOUP_X86 && SOUP_BITS == 64
#include <emmintrin.h>
#endif

#include "base.hpp"
#include "bitutil.hpp"
#include "CpuInfo.hpp"
#include "Pattern.hpp"

namespace soup
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
		auto data = sig.bytes.data();
		auto length = sig.bytes.size();
		SOUP_IF_UNLIKELY (length == 0)
		{
			return base;
		}
#if SOUP_X86 && SOUP_BITS == 64
		SOUP_IF_LIKELY (data[0].has_value())
		{
			if (CpuInfo::get().supportsSSE2())
			{
				return scanSimd(sig);
			}
		}
#endif
		for (uintptr_t i = 0; i < size - length; ++i)
		{
			if (pattern_matches(base.add(i).as<uint8_t*>(), data, length))
			{
				return base.add(i);
			}
		}
		return nullptr;
	}

#if SOUP_X86 && SOUP_BITS == 64
	Pointer Range::scanSimd(const Pattern& sig) const noexcept
	{
		auto data = sig.bytes.data();
		auto length = sig.bytes.size();
		const auto match = _mm_set1_epi8(data[0].value());
		for (uintptr_t i = 0; i < size - length; i += 16)
		{
			int mask = _mm_movemask_epi8(_mm_cmpeq_epi8(match, _mm_loadu_si128(base.add(i).as<__m128i*>())));
			while (mask != 0)
			{
				auto j = bitutil::getLeastSignificantSetBit(mask);
				if (pattern_matches(base.add(i).add(j).add(1).as<uint8_t*>(), data + 1, length - 1))
				{
					return base.add(i).add(j);
				}
				bitutil::unsetLeastSignificantSetBit(mask);
			}
		}
		return nullptr;
	}
#endif

	std::vector<Pointer> Range::scanAll(const Pattern& sig, unsigned int limit) const
	{
		auto data = sig.bytes.data();
		auto length = sig.bytes.size();
		SOUP_IF_UNLIKELY (length == 0)
		{
			return {};
		}
#if SOUP_X86 && SOUP_BITS == 64
		SOUP_IF_LIKELY (data[0].has_value())
		{
			if (CpuInfo::get().supportsSSE2())
			{
				return scanAllSimd(sig, limit);
			}
		}
#endif
		std::vector<Pointer> result{};
		for (uintptr_t i = 0; i < size - length; ++i)
		{
			if (pattern_matches(base.add(i).as<uint8_t*>(), data, length))
			{
				result.emplace_back(base.add(i));
				if (result.size() >= limit)
				{
					break;
				}
			}
		}
		return result;
	}

#if SOUP_X86 && SOUP_BITS == 64
	std::vector<Pointer> Range::scanAllSimd(const Pattern& sig, unsigned int limit) const
	{
		auto data = sig.bytes.data();
		auto length = sig.bytes.size();
		const auto match = _mm_set1_epi8(data[0].value());
		std::vector<Pointer> result{};
		for (uintptr_t i = 0; i < size - length; i += 16)
		{
			int mask = _mm_movemask_epi8(_mm_cmpeq_epi8(match, _mm_loadu_si128(base.add(i).as<__m128i*>())));
			while (mask != 0)
			{
				auto j = bitutil::getLeastSignificantSetBit(mask);
				if (pattern_matches(base.add(i).add(j).add(1).as<uint8_t*>(), data + 1, length - 1))
				{
					result.emplace_back(base.add(i).add(j));
					if (result.size() >= limit)
					{
						return result;
					}
				}
				bitutil::unsetLeastSignificantSetBit(mask);
			}
		}
		return result;
	}
#endif
}
