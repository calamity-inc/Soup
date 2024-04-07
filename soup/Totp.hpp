#pragma once

#include "Hotp.hpp"

NAMESPACE_SOUP
{
	// Time-based one-time password
	struct Totp : public Hotp
	{
		using Hotp::Hotp;

		[[nodiscard]] std::string getQrCodeUri(const std::string& label, const std::string& issuer = {}) const;

		[[nodiscard]] int getValue() const;
		[[nodiscard]] int getValue(uint64_t t) const;
	};
}
