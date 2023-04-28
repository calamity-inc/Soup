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
				if (*flags == 's')
				{
					res |= RE_DOTALL;
				}
				else if (*flags == 'm')
				{
					res |= RE_MULTILINE;
				}
				else if (*flags == 'D')
				{
					res |= RE_DOLLAR_ENDONLY;
				}
			}
			return res;
		}

		[[nodiscard]] static std::string unparseFlags(uint16_t flags);
	};
}
