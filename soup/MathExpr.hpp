#pragma once

#include <string>

#include "Optional.hpp"

namespace soup
{
	// Evaluates expressions rougly in this format: (?:\d+\s*[+\-*\/%]\s*)+\d+
	// TODO: Handle parens
	// TODO: Handle unary operators (e.g. -3)
	// TODO: Add exponent operator (^)
	// TODO: Add constants like 'pi'
	// TODO: Add functions like 'sqrt'
	// TODO: Use Bigint to allow for arbitrary-bit integers
	struct MathExpr
	{
		static Optional<int64_t> evaluate(const std::string& str);
	};
}
