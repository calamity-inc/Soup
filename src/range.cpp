#include "range.hpp"

#include "pattern.hpp"
#include "platform.hpp"

namespace soup
{
	range::range(pointer base, size_t size) noexcept
		: base(base), size(size)
	{
	}

	pointer range::end() const noexcept
	{
		return base.add(size);
	}

	bool range::contains(pointer h) const noexcept
	{
		return h.as<uintptr_t>() >= base.as<uintptr_t>() && h.as<uintptr_t>() <= end().as<uintptr_t>();
	}

	bool range::pattern_matches(uint8_t* target, const std::optional<uint8_t>* sig, size_t length) noexcept
	{
#if SOUP_PLATFORM_WINDOWS
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
#if SOUP_PLATFORM_WINDOWS
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
		return false;
#endif
	}

	pointer range::scan(const pattern& sig) const noexcept
	{
		auto data = sig.m_bytes.data();
		auto length = sig.m_bytes.size();
		for (std::uintptr_t i = 0; i < size - length; ++i)
		{
			if (pattern_matches(base.add(i).as<std::uint8_t*>(), data, length))
			{
				return base.add(i);
			}
		}
		return nullptr;
	}

	std::vector<pointer> range::scan_all(const pattern& sig, unsigned int limit) const
	{
		std::vector<pointer> result;

		auto data = sig.m_bytes.data();
		auto length = sig.m_bytes.size();
		for (std::uintptr_t i = 0; i < size - length; ++i)
		{
			if (pattern_matches(base.add(i).as<std::uint8_t*>(), data, length))
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
}
