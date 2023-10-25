#pragma once

#include <cstdint>
#include <string>

namespace soup
{
	// HMAC-based one-time password
	class Hotp
	{
	public:
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

		[[nodiscard]] int getValue(uint64_t counter) const; // digits = 6
		[[nodiscard]] int getValue(uint64_t counter, uint8_t digits) const; // digits can be 6-8
	protected:
		[[nodiscard]] int getValueRaw(uint64_t counter) const;
	};
}
