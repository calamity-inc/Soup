#include "Regex.hpp"

#include "base.hpp"
#include "RegexConstraintTransitionable.hpp"
#include "RegexMatcher.hpp"

#define REGEX_DEBUG_MATCH false

#if REGEX_DEBUG_MATCH
#include <iostream>
#endif

namespace soup
{
	bool Regex::matches(const std::string& str) const noexcept
	{
		return matches(str.cbegin(), str.cend());
	}

	bool Regex::matches(std::string::const_iterator it, std::string::const_iterator end) const noexcept
	{
		RegexMatcher m(*this, it, end);
		while (m.c != nullptr)
		{
#if REGEX_DEBUG_MATCH
			std::cout << m.c->toString() << ": ";
#endif

			if (m.c->rollback_transition)
			{
#if REGEX_DEBUG_MATCH
				std::cout << "saved rollback; ";
#endif
				m.save(m.c->rollback_transition);
			}

			// Matches?
			if (m.c->matches(m.it, m.end))
			{
#if REGEX_DEBUG_MATCH
				std::cout << "matched\n";
#endif
				m.c = m.c->success_transition;
				SOUP_ASSERT(reinterpret_cast<uintptr_t>(m.c) != 1);
				continue;
			}

			// Rollback?
			if (!m.rollback_points.empty())
			{
#if REGEX_DEBUG_MATCH
				std::cout << "did not match, rolling back\n";
#endif
				m.restore();
				if (reinterpret_cast<uintptr_t>(m.c) == 1)
				{
#if REGEX_DEBUG_MATCH
					std::cout << "rollback says we should succeed now\n";
#endif
					return true;
				}
				continue;
			}

			// Oh well
#if REGEX_DEBUG_MATCH
			std::cout << "no matchy\n";
#endif
			return false;
		}
		return true;
	}
}
