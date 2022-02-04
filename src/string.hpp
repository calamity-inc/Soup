#pragma once

#include <cstdint>

namespace soup
{
	class string
	{
	private:
		template <typename Str, typename Int, uint8_t Base>
		[[nodiscard]] static Str from_int_impl_ascii(Int i)
		{
			if (i == 0)
			{
				return Str(1, '0');
			}
			const bool neg = (i < 0);
			if (neg)
			{
				i = i * -1;
			}
			Str res{};
			for (; i != 0; i /= Base)
			{
				const auto digit = (i % Base);
				res.insert(0, 1, '0' + digit);
			}
			if (neg)
			{
				res.insert(0, 1, '-');
			}
			return res;
		}

	public:
		template <typename Str, typename Int>
		[[nodiscard]] static Str decimal(Int i)
		{
			return from_int_impl_ascii<Str, Int, 10>(i);
		}

		template <typename Str, typename Int>
		[[nodiscard]] static Str binary(Int i)
		{
			return from_int_impl_ascii<Str, Int, 2>(i);
		}
	};
}
