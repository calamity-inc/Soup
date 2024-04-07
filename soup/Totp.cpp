#include "Totp.hpp"

#include "base32.hpp"
#include "Hotp.hpp"
#include "time.hpp"

NAMESPACE_SOUP
{
	std::string Totp::getQrCodeUri(const std::string& label, const std::string& issuer) const
	{
		// As per https://github.com/google/google-authenticator/wiki/Key-Uri-Format
		std::string uri = "otpauth://totp/";
		if (!issuer.empty())
		{
			uri.append(issuer);
			uri.push_back(':');
		}
		uri.append(label);
		uri.append("?secret=");
		uri.append(base32::encode(secret, false));
		if (!issuer.empty())
		{
			uri.append("&issuer=");
			uri.append(issuer);
		}
		return uri;
	}

	int Totp::getValue() const
	{
		return getValue(time::unixSeconds());
	}

	int Totp::getValue(uint64_t t) const
	{
		return Hotp::getValue(t / 30);
	}
}
