#pragma once

#include <algorithm> // swap
#include <string>

#include "base.hpp"
#include "string.hpp"
#include "string_literal.hpp"

namespace soup
{
	template <size_t Len>
	class obfus_string
	{
	private:
		char data[Len];
		bool runtime_once = false;

	public:
		consteval obfus_string(const char(&in)[Len])
		{
			// rot13
			for (size_t i = 0; i != Len; ++i)
			{
				data[i] = string::rot13(in[i]);
			}

			// flip bits
			for (size_t i = 0; i != Len; ++i)
			{
				data[i] ^= 96u;
			}

			// mirror
			for (size_t i = 0, j = Len - 1; i != Len; ++i, --j)
			{
				std::swap(data[i], data[j]);
			}
		}

	private:
		SOUP_NOINLINE void runtime_access() noexcept
		{
			if (runtime_once)
			{
				return;
			}
			runtime_once = true;

			// mirror
			for (size_t i = 0, j = Len - 1; i != Len; ++i, --j)
			{
				std::swap(data[i], data[j]);
			}

			// flip bits
			for (size_t i = 0; i != Len; ++i)
			{
				data[i] ^= 96u;
			}

			// rot13
			for (size_t i = 0; i != Len; ++i)
			{
				data[i] = string::rot13(data[i]);
			}
		}

	public:
		[[nodiscard]] std::string str() noexcept
		{
			runtime_access();
			return std::string(data, Len);
		}

		[[nodiscard]] operator std::string() noexcept
		{
			return str();
		}

		[[nodiscard]] const char* c_str() noexcept
		{
			runtime_access();
			return data;
		}

		[[nodiscard]] operator const char* () noexcept
		{
			return c_str();
		}

		[[nodiscard]] bool operator==(const std::string& b) noexcept
		{
			return str() == b;
		}

		[[nodiscard]] bool operator!=(const std::string& b) noexcept
		{
			return !operator==(b);
		}

		template <size_t BLen>
		[[nodiscard]] bool operator==(const obfus_string<BLen>& b) noexcept
		{
			return str() == b.str();
		}

		template <size_t BLen>
		[[nodiscard]] bool operator!=(const obfus_string<BLen>& b) noexcept
		{
			return !operator==(b);
		}

		[[nodiscard]] bool operator==(const char* b) noexcept
		{
			return strcmp(c_str(), b) == 0;
		}

		[[nodiscard]] bool operator!=(const char* b) noexcept
		{
			return !operator==(b);
		}

		friend std::ostream& operator<<(std::ostream& os, obfus_string& str)
		{
			os << str.str();
			return os;
		}
	};

	namespace literals
	{
		template <string_literal Str>
		consteval auto operator "" _obfus()
		{
			return obfus_string<Str.size()>(Str.data);
		}
	}
}
