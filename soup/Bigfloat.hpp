#pragma once

#include "Bigint.hpp"

NAMESPACE_SOUP
{
	// Not really meant to be used:
	// a) Can't represent 0.01
	// b) 0.5 + 0.5 = 0.10
	struct Bigfloat
	{
		Bigint i;
		Bigint f;

		[[nodiscard]] Bigfloat operator +(const Bigfloat& b) const noexcept;

		[[nodiscard]] std::string toString() const noexcept;
	};
}
