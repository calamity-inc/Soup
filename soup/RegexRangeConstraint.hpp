#pragma once

#include "RegexConstraintTransitionable.hpp"

#include "base.hpp"
#include "BigBitset.hpp"
#include "RegexMatcher.hpp"

namespace soup
{
	struct RegexRangeConstraint : public RegexConstraintTransitionable
	{
		BigBitset<0x100 / 8> mask{};

		RegexRangeConstraint(std::string::const_iterator& it, std::string::const_iterator end)
		{
			char range_begin = 0;
			while (++it != end && *it != ']')
			{
				if (*it == '-')
				{
					SOUP_IF_UNLIKELY (++it == end)
					{
						break;
					}
					if (range_begin <= *it)
					{
						for (char c = range_begin; c != *it; ++c)
						{
							mask.enable(c);
						}
					}
				}
				mask.enable(*it);
				range_begin = (*it) + 1;
			}
		}

		[[nodiscard]] bool matches(RegexMatcher& m) const noexcept final
		{
			if (m.it == m.end)
			{
				return false;
			}
			if (!mask.get(*m.it))
			{
				return false;
			}
			++m.it;
			return true;
		}

		[[nodiscard]] std::string toString() const noexcept final
		{
			std::string str(1, '[');
			uint16_t range_begin = 0x100;
			for (uint16_t i = 0; i != 0x100; ++i)
			{
				if (mask.get(i))
				{
					if (range_begin == 0x100)
					{
						range_begin = i;
					}
				}
				else
				{
					if (range_begin != 0x100)
					{
						const uint8_t range_end = i;
						const uint8_t range_len = (range_end - range_begin);
						if (range_len > 3)
						{
							str.push_back(range_begin);
							str.push_back('-');
							str.push_back(range_end - 1);
						}
						else
						{
							for (uint16_t j = range_begin; j != range_end; ++j)
							{
								str.push_back(j);
							}
						}
						range_begin = 0x100;
					}
				}
			}
			if (range_begin != 0x100)
			{
				constexpr uint16_t range_end = 0x100;
				const uint8_t range_len = (range_end - range_begin);
				if (range_len > 3)
				{
					str.push_back(range_begin);
					str.push_back('-');
					str.push_back((char)(range_end - 1));
				}
				else
				{
					for (uint16_t j = range_begin; j != range_end; ++j)
					{
						str.push_back(j);
					}
				}
			}
			str.push_back(']');
			return str;
		}
	};
}
