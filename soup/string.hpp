#pragma once

#include <algorithm> // std::transform
#include <cctype> // std::tolower
#include <cstdint>
#include <cstring> // strlen
#include <optional>
#include <string>
#include <vector>

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

		template <typename T>
		[[nodiscard]] static constexpr bool isNumberChar(const T c) noexcept
		{
			return c >= '0' && c <= '9';
		}

		template <typename T>
		[[nodiscard]] static constexpr bool isSpace(const T c) noexcept
		{
			return c == ' ' || c == '\t' || c == '\n' || c == '\r';
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

		template <typename IntT, typename CharT>
		[[nodiscard]] static IntT toInt(const CharT*& it) noexcept
		{
			IntT val = 0;
			IntT max = 0;
			IntT prev_max = 0;
			while (true)
			{
				const CharT c = *it++;
				if (!isNumberChar(c))
				{
					break;
				}
				val *= 10;
				val += (c - '0');
				max *= 10;
				max += 9;
				if (max < prev_max)
				{
					break;
				}
				prev_max = max;
			}
			return val;
		}

		template <typename IntT, typename StringView>
		[[nodiscard]] static std::optional<IntT> toInt(const StringView& str) noexcept
		{
			using CharT = typename StringView::value_type;

			bool neg = false;
			auto it = str.cbegin();
			if (it == str.cend())
			{
				return std::nullopt;
			}
			switch (*it)
			{
			case '-':
				neg = true;
				[[fallthrough]];
			case '+':
				if (++it == str.cend())
				{
					return std::nullopt;
				}
			}
			if (!isNumberChar(*it))
			{
				return std::nullopt;
			}
			const CharT* it_ = &*it;
			IntT val = toInt<IntT, CharT>(it_);
			if (neg)
			{
				val *= -1;
			}
			return std::optional<IntT>(std::move(val));
		}

		template <typename IntT>
		[[nodiscard]] static std::optional<IntT> toInt(const std::string_view& str) noexcept
		{
			return toInt<IntT, std::string_view>(str);
		}

		template <typename IntT>
		[[nodiscard]] static std::optional<IntT> toInt(const std::wstring_view& str) noexcept
		{
			return toInt<IntT, std::wstring_view>(str);
		}

		template <typename IntT>
		[[nodiscard]] static IntT toInt(const std::string_view& str, IntT default_value) noexcept
		{
			auto res = toInt<IntT>(str);
			if (res.has_value())
			{
				return res.value();
			}
			return default_value;
		}

		template <typename IntT>
		[[nodiscard]] static IntT toInt(const std::wstring_view& str, IntT default_value) noexcept
		{
			auto res = toInt<IntT>(str);
			if (res.has_value())
			{
				return res.value();
			}
			return default_value;
		}

		// string mutation

		template <class S>
		static void replace_all(S& str, const S& from, const S& to) noexcept
		{
			size_t start_pos = 0;
			while ((start_pos = str.find(from, start_pos)) != S::npos)
			{
				str.replace(start_pos, from.length(), to);
				start_pos += to.length();
			}
		}

		static void replace_all(std::string& str, const std::string& from, const std::string& to) noexcept
		{
			return replace_all<std::string>(str, from, to);
		}

		static void replace_all(std::wstring& str, const std::wstring& from, const std::wstring& to) noexcept
		{
			return replace_all<std::wstring>(str, from, to);
		}

		template <typename S>
		static size_t len(S str)
		{
			if constexpr (std::is_same_v<S, char> || std::is_same_v<S, wchar_t>)
			{
				return 1;
			}
			else if constexpr (std::is_pointer_v<S>)
			{
				return strlen(str);
			}
			else
			{
				return str.size();
			}
		}

		template <typename S, typename D>
		[[nodiscard]] static std::vector<S> explode(const S& str, D delim)
		{
			std::vector<S> res{};
			if (!str.empty())
			{
				size_t prev = 0;
				size_t del_pos;
				while ((del_pos = str.find(delim, prev)) != std::string::npos)
				{
					res.emplace_back(str.substr(prev, del_pos - prev));
					prev = del_pos + len(delim);
				}
				auto remain_len = (str.length() - prev);
				if (remain_len != 0)
				{
					res.emplace_back(str.substr(prev, remain_len));
				}
				else
				{
					if constexpr (std::is_same_v<D, char> || std::is_same_v<D, wchar_t>)
					{
						if (str.at(str.length() - 1) == delim)
						{
							res.emplace_back(S{});
						}
					}
				}
			}
			return res;
		}

		template <typename S, typename C>
		static S lpad(S&& str, size_t desired_len, C pad_char)
		{
			lpad(str, desired_len, std::move(pad_char));
			return str;
		}

		template <typename S, typename C>
		static void lpad(S& str, size_t desired_len, C pad_char)
		{
			if (auto diff = desired_len - str.length(); diff > 0)
			{
				str.insert(0, diff, pad_char);
			}
		}

		template <typename S, typename C>
		static S rpad(S&& str, size_t desired_len, C pad_char)
		{
			rpad(str, desired_len, std::move(pad_char));
			return str;
		}

		template <typename S, typename C>
		static void rpad(S& str, size_t desired_len, C pad_char)
		{
			if (auto diff = desired_len - str.length(); diff > 0)
			{
				str.append(diff, pad_char);
			}
		}

		// example:
		// in str = "a b c"
		// target = " "
		// out str = "abc"
		template <typename T>
		static void erase(T& str, const T& target)
		{
			for (size_t i = 0; i = str.find(target, i), i != T::npos; )
			{
				str.erase(i, target.size());
			}
		}

		// example:
		// in str = "a b c"
		// target = " "
		// out str = "a"
		template <typename T>
		static void limit(T& str, const T& target)
		{
			for (size_t i = 0; i = str.find(target, i), i != T::npos; )
			{
				str.erase(i);
			}
		}

		static void listAppend(std::string& str, std::string&& add);

		// char mutation

		template <typename Str>
		static void lower(Str& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c)
			{
				return std::tolower(c);
			});
		}

		template <typename Str>
		[[deprecated]] static void toLower(Str& str)
		{
			return lower(str);
		}

		template <typename Str>
		static void upper(Str& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c)
			{
				return std::toupper(c);
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
