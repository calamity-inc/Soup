#pragma once

#include <string>

namespace soup
{
	// Adapted from https://homes.esat.kuleuven.be/~bosselae/ripemd160.html

	[[nodiscard]] std::string ripemd160(const std::string& in);
}
