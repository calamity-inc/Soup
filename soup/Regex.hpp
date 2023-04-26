#pragma once

#include "RegexGroup.hpp"
#include "RegexMatchResult.hpp"

namespace soup
{
	struct Regex
	{
		enum Flags : uint16_t
		{
			single_line = (1 << 0), // 's' - '.' also matches '\n' - a.k.a. "dotall"
			multi_line = (1 << 1), // 'm' - '^' and '$' also match start and end of lines, respectively
			dollar_end_only = (1 << 2), // 'D' - '$' only matches end of pattern, not '\n' - ignored if multi_line flag is set
		};

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
			: Regex(b.toString()) // BUG: Flags are ignored
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

		[[nodiscard]] static constexpr uint16_t parseFlags(const char* flags)
		{
			uint16_t res = 0;
			for (; *flags != '\0'; ++flags)
			{
				if (*flags == 's')
				{
					res |= single_line;
				}
				else if (*flags == 'm')
				{
					res |= multi_line;
				}
				else if (*flags == 'D')
				{
					res |= dollar_end_only;
				}
			}
			return res;
		}
	};
}
