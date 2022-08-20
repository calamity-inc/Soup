#include "Keystore.hpp"

#include "pem.hpp"
#include "X509Certificate.hpp"

namespace soup
{
	void Keystore::loadCaCerts(std::istream& is)
	{
		std::string ca_common_name{};
		std::string ca_pem{};
		for (std::string line{}; std::getline(is, line); )
		{
			if (line.empty()
				|| line.at(0) == '#'
				)
			{
				if (!ca_common_name.empty())
				{
					addCa(std::move(ca_common_name), std::move(ca_pem));
					ca_common_name.clear();
					ca_pem.clear();
				}
			}
			else if (line.at(0) != '=')
			{
				if (ca_common_name.empty())
				{
					ca_common_name = std::move(line);
				}
				else
				{
					ca_pem.append(std::move(line));
				}
			}
		}
		if (!ca_common_name.empty())
		{
			addCa(std::move(ca_common_name), std::move(ca_pem));
		}
	}

	void Keystore::addCa(std::string&& common_name, std::string&& pem)
	{
		X509Certificate xcert;
		if (xcert.fromBinary(pem::decode(std::move(pem)))
			&& !xcert.key.n.isZero()
			)
		{
			addCa(std::move(common_name), std::move(xcert.key));
		}
	}

	void Keystore::addCa(std::string&& common_name, RsaPublicKey&& key)
	{
		data.emplace(std::move(common_name), std::move(key));
	}

	const RsaPublicKey* Keystore::findCommonName(const std::string& cn) const
	{
		auto i = data.find(cn);
		if (i == data.end())
		{
			return nullptr;
		}
		return &i->second;
	}
}
