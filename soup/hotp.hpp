#pragma once

#include <string>

namespace soup
{
	// HMAC-based one-time password
	[[nodiscard]] int hotp(const std::string& secret, uint64_t counter, uint8_t digits = 6);
}
