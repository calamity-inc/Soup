#pragma once

#include "RegexFlags.hpp"
#include "RegexGroup.hpp"
#include "RegexMatchResult.hpp"

namespace soup
{
	struct Regex
	{
		RegexGroup group;

		Regex(const std::string& pattern, const char* flags)
			: Regex(pattern.cbegin(), pattern.cend(), parseFlags(flags))
		{
		}

		Regex(const std::string& pattern, uint16_t flags = 0)
			: Regex(pattern.cbegin(), pattern.cend(), flags)
		{
		}

		Regex(std::string::const_iterator it, std::string::const_iterator end, uint16_t flags = 0)
			: group(it, end, flags)
		{
		}

		Regex(const Regex& b)
			: Regex(b.toString(), b.getFlags())
		{
		}

		Regex(Regex&&) = default;

		[[nodiscard]] bool matches(const std::string& str) const noexcept;
		[[nodiscard]] bool matches(std::string::const_iterator it, std::string::const_iterator end) const noexcept;

		[[nodiscard]] bool matchesFully(const std::string& str) const noexcept;
		[[nodiscard]] bool matchesFully(std::string::const_iterator it, std::string::const_iterator end) const noexcept;

		[[nodiscard]] RegexMatchResult match(const std::string& str) const noexcept;
		[[nodiscard]] RegexMatchResult match(std::string::const_iterator it, std::string::const_iterator end) const noexcept;
		[[nodiscard]] RegexMatchResult match(std::string::const_iterator it, std::string::const_iterator begin, std::string::const_iterator end) const noexcept;

		[[nodiscard]] RegexMatchResult search(const std::string& str) const noexcept;
		[[nodiscard]] RegexMatchResult search(std::string::const_iterator it, std::string::const_iterator end) const noexcept;

		void replaceAll(std::string& str, const std::string& replacement) const;

		[[nodiscard]] std::string toString() const noexcept
		{
			return group.toString();
		}

		[[nodiscard]] std::string toFullString() const noexcept
		{
			std::string str(1, '/');
			str.append(toString());
			str.push_back('/');
			str.append(getFlagsString());
			return str;
		}

		[[nodiscard]] uint16_t getFlags() const noexcept
		{
			return group.getFlags();
		}

		[[nodiscard]] std::string getFlagsString() const noexcept
		{
			return unparseFlags(group.getFlags());
		}

		[[nodiscard]] static constexpr uint16_t parseFlags(const char* flags)
		{
			uint16_t res = 0;
			for (; *flags != '\0'; ++flags)
			{
				if (*flags == 'm')
				{
					res |= RE_MULTILINE;
				}
				else if (*flags == 's')
				{
					res |= RE_DOTALL;
				}
				else if (*flags == 'i')
				{
					res |= RE_INSENSITIVE;
				}
				else if (*flags == 'u')
				{
					res |= RE_UNICODE;
				}
				else if (*flags == 'U')
				{
					res |= RE_UNGREEDY;
				}
				else if (*flags == 'D')
				{
					res |= RE_DOLLAR_ENDONLY;
				}
				else if (*flags == 'n')
				{
					res |= RE_EXPLICIT_CAPTURE;
				}
			}
			return res;
		}

		[[nodiscard]] static std::string unparseFlags(uint16_t flags);
	};

	namespace literals
	{
		inline Regex operator ""_r(const char* str, size_t len)
		{
			return Regex(std::string(str, len));
		}
	}
}
