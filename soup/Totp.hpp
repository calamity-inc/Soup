#pragma once

#include "Hotp.hpp"

namespace soup
{
	// Time-based one-time password
	struct Totp : public Hotp
	{
		[[nodiscard]] std::string getQrCodeUri(const std::string& label, const std::string& issuer = {}) const;

		[[nodiscard]] int getValue();
		[[nodiscard]] int getValue(uint64_t t);
	};
}
