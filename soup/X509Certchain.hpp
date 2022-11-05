#pragma once

#include <vector>

#include "fwd.hpp"
#include "X509Certificate.hpp"

namespace soup
{
	struct X509Certchain
	{
		std::vector<X509Certificate> certs{};

		bool fromDer(const std::vector<std::string>& vec);
		bool fromPem(const std::string& str);

		[[nodiscard]] bool verify(const std::string& domain, const Keystore& ks) const;
		[[nodiscard]] bool isValidForDomain(const std::string& domain) const;
		[[nodiscard]] bool verify(const Keystore& ks) const;
		[[nodiscard]] bool verifyTrust(const Keystore& ks) const;
		[[nodiscard]] bool isAnyInKeystore(const Keystore& ks) const;
		[[nodiscard]] bool verifySignatures() const;
	};
}
