#include "X509Certchain.hpp"

#include <cstring> // strlen

#include "Keystore.hpp"
#include "pem.hpp"
#include "string.hpp"

namespace soup
{
	bool X509Certchain::fromDer(const std::vector<std::string>& vec)
	{
		for (auto& cert : vec)
		{
			X509Certificate xcert{};
			if (!xcert.fromBinary(cert))
			{
				return false;
			}
			certs.emplace_back(std::move(xcert));
		}
		return !certs.empty();
	}

	bool X509Certchain::fromPem(const std::string& str)
	{
		for (const auto& der : pem::decodeChain(str))
		{
			X509Certificate xcert{};
			if (!xcert.fromBinary(der))
			{
				return false;
			}
			certs.emplace_back(std::move(xcert));
		}
		return !certs.empty();
	}

	bool X509Certchain::verify(const std::string& domain, const Keystore& ks) const
	{
		return isValidForDomain(domain)
			&& verify(ks);
	}

	bool X509Certchain::isValidForDomain(const std::string& domain) const
	{
		return certs.at(0).subject.getCommonName() == domain;
	}

	bool X509Certchain::verify(const Keystore& ks) const
	{
		return verifyTrust(ks)
			&& verifySignatures()
			;
	}

	bool X509Certchain::verifyTrust(const Keystore& ks) const
	{
		if (!certs.empty())
		{
			if (isAnyInKeystore(ks))
			{
				return true;
			}

			const auto& root = certs.back();
			if (auto entry = ks.findCommonName(root.issuer.getCommonName()))
			{
				if (root.verify(*entry))
				{
					return true;
				}
			}
		}
		return false;
	}

	bool X509Certchain::isAnyInKeystore(const Keystore& ks) const
	{
		for (auto i = certs.rbegin(); i != certs.rend(); ++i)
		{
			if (auto entry = ks.findCommonName(i->subject.getCommonName()))
			{
				if (entry->n == i->key.n)
				{
					return true;
				}
			}
		}
		return false;
	}

	bool X509Certchain::verifySignatures() const
	{
		if (certs.size() > 1)
		{
			for (auto i = certs.begin(); i != certs.end() - 1; ++i)
			{
				if (!i->verify(*(i + 1)))
				{
					return false;
				}
			}
		}
		return true;
	}
}
