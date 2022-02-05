#pragma once

#include <cstdint>
#include <string>

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
		template <typename Str = std::string, typename Int>
		[[nodiscard]] static Str decimal(Int i)
		{
			return from_int_impl_ascii<Str, Int, 10>(i);
		}

		template <typename Str = std::string, typename Int>
		[[nodiscard]] static Str binary(Int i)
		{
			return from_int_impl_ascii<Str, Int, 2>(i);
		}

		template <typename Int>
		[[nodiscard]] static std::string hex_upper(Int i)
		{
			return fromIntWithMap<std::string, Int, 16>(i, from_int_map_hex_upper);
		}

		template <typename Int>
		[[nodiscard]] static std::string hex_lower(Int i)
		{
			return fromIntWithMap<std::string, Int, 16>(i, from_int_map_hex_lower);
		}

		static constexpr const char* from_int_map_hex_upper = "0123456789ABCDEF";
		static constexpr const char* from_int_map_hex_lower = "0123456789abcdef";

		template <typename Str, typename Int, uint8_t Base>
		[[nodiscard]] static Str fromIntWithMap(Int i, const typename Str::value_type* map)
		{
			if (i == 0)
			{
				return Str(1, map[0]);
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
				res.insert(0, 1, map[digit]);
			}
			if (neg)
			{
				res.insert(0, 1, '-');
			}
			return res;
		}
	};
}
