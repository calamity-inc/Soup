#pragma once

#include <vector>

#include "fwd.hpp"
#include "x509_certificate.hpp"

namespace soup
{
	struct certchain
	{
		std::vector<x509_certificate> certs{};

		bool fromDer(const std::vector<std::string>& vec);
		bool fromPem(std::string str);

		[[nodiscard]] bool verify(const std::string& domain, const keystore& ks) const;
		[[nodiscard]] bool isValidForDomain(const std::string& domain) const;
		[[nodiscard]] bool verify(const keystore& ks) const;
		[[nodiscard]] bool verifyTrust(const keystore& ks) const;
		[[nodiscard]] bool isAnyInKeystore(const keystore& ks) const;
		[[nodiscard]] bool verifySignatures() const;
	};
}
