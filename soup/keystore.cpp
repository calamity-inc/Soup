#include "keystore.hpp"

#include "pem.hpp"
#include "x509_certificate.hpp"

namespace soup
{
	void keystore::loadCaCerts(std::basic_istream<char, std::char_traits<char>>& is)
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

	void keystore::addCa(std::string&& common_name, std::string&& pem)
	{
		x509_certificate xcert;
		if (xcert.fromBinary(pem::decode(std::move(pem)))
			&& !xcert.key.n.isZero()
			)
		{
			addCa(std::move(common_name), std::move(xcert.key));
		}
	}

	void keystore::addCa(std::string&& common_name, rsa::key_public&& key)
	{
		data.emplace(std::move(common_name), std::move(key));
	}

	const rsa::key_public* keystore::findCommonName(const std::string& cn) const
	{
		auto i = data.find(cn);
		if (i == data.end())
		{
			return nullptr;
		}
		return &i->second;
	}
}
