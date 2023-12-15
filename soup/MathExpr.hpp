#pragma once

#include <string>

namespace soup
{
	// Evaluates expressions rougly in this format: (?:\d+\s*[+\-*\/]\s*)+\d+
	// Soup's Language Stack handles the order of operations, so e.g. 1 + 2 * 3 evaluates to 7.
	// TODO: Handle parens
	// TODO: Handle unary operators (e.g. -3)
	// TODO: Add modulo operator (%)
	// TODO: Add exponent operator (^)
	// TODO: Add constants like 'pi'
	// TODO: Add functions like 'sqrt'
	// TODO: Use Bigint to allow for arbitrary-bit integers
	struct MathExpr
	{
		static int64_t evaluate(const std::string& str);
	};
}
