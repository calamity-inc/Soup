#pragma once

#include <string>

#include "spaceship.hpp"

namespace soup
{
	[[nodiscard]] strong_ordering version_compare(const std::string& in_a, const std::string& in_b);
}
