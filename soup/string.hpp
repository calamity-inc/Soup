#pragma once

#include <algorithm> // std::transform
#include <cctype> // std::tolower
#include <cstdint>
#include <string>

namespace soup
{
	class string
	{
		// from int

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
		[[nodiscard]] static std::string hex(Int i)
		{
			return fromIntWithMap<std::string, Int, 16>(i, charset_hex);
		}

		template <typename Int>
		[[nodiscard]] static std::string hex_lower(Int i)
		{
			return fromIntWithMap<std::string, Int, 16>(i, charset_hex_lower);
		}

		static constexpr const char* charset_hex = "0123456789ABCDEF";
		static constexpr const char* charset_hex_lower = "0123456789abcdef";

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

		// char attributes

		template <typename T>
		[[nodiscard]] static constexpr bool isUppercaseLetter(const T c) noexcept
		{
			return c >= 'A' && c <= 'Z';
		}

		template <typename T>
		[[nodiscard]] static constexpr bool isLowercaseLetter(const T c) noexcept
		{
			return c >= 'a' && c <= 'z';
		}

		template <typename T>
		[[nodiscard]] static constexpr bool isLetter(const T c) noexcept
		{
			return isUppercaseLetter(c) || isLowercaseLetter(c);
		}

		// conversions

		[[nodiscard]] static std::string bin2hex(const std::string& str)
		{
			return bin2hexImpl(str, charset_hex);
		}

		[[nodiscard]] static std::string bin2hexLower(const std::string& str)
		{
			return bin2hexImpl(str, charset_hex_lower);
		}
		
		[[nodiscard]] static std::string bin2hexImpl(std::string str, const char* map)
		{
			std::string res{};
			res.reserve(str.size() * 2);
			for (const auto& c : str)
			{
				res.push_back(map[(unsigned char)c >> 4]);
				res.push_back(map[c & 0b1111]);
			}
			return res;
		}

		// string mutation

		template <typename T>
		static void erase(T& str, const T& target)
		{
			for (size_t i = 0; i = str.find(target, i), i != T::npos; )
			{
				str.erase(i, target.size());
			}
		}

		// char mutation

		template <typename Str>
		static void toLower(Str& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), [](auto c)
			{
				return std::tolower(c);
			});
		}

		[[nodiscard]] static constexpr char rot13(char c) noexcept
		{
			if (isUppercaseLetter(c))
			{
				char val = (c - 'A');
				val += 13;
				if (val >= 26)
				{
					val -= 26;
				}
				return (val + 'A');
			}
			if (isLowercaseLetter(c))
			{
				char val = (c - 'a');
				val += 13;
				if (val >= 26)
				{
					val -= 26;
				}
				return (val + 'a');
			}
			return c;
		}

		// file

		[[nodiscard]] static std::string fromFile(const std::string& file);
	};
}
