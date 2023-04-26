#pragma once

#include "RegexGroup.hpp"
#include "RegexMatch.hpp"

namespace soup
{
	struct Regex
	{
		enum Flags : uint16_t
		{
			single_line = (1 << 0), // 's' - '.' also matches '\n'
			multi_line = (1 << 1), // 'm' - '^' also matches start of line
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

		[[nodiscard]] std::optional<RegexMatch> match(const std::string& str) const noexcept;
		[[nodiscard]] std::optional<RegexMatch> match(std::string::const_iterator it, std::string::const_iterator end) const noexcept;

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
			}
			return res;
		}
	};
}
