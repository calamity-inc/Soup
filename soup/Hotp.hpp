#pragma once

#include <string>

namespace soup
{
	// HMAC-based one-time password
	struct Hotp
	{
		std::string secret{};

		Hotp()
			: secret(generateSecret())
		{
		}

		Hotp(std::string secret)
			: secret(std::move(secret))
		{
		}

		[[nodiscard]] static std::string generateSecret(size_t bytes = 16); // bytes should be a multiple of 8

		[[nodiscard]] int getValue(uint64_t counter, uint8_t digits = 6) const;
	};
}
