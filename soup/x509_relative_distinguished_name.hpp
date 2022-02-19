#pragma once

#include <string>
#include <utility>
#include <vector>

#include "oid.hpp"

namespace soup
{
	struct x509_relative_distinguished_name : public std::vector<std::pair<oid, std::string>>
	{
		[[nodiscard]] std::string get(const oid& target) const
		{
			for (const auto& kv : *this)
			{
				if (kv.first == target)
				{
					return kv.second;
				}
			}
			return {};
		}

		[[nodiscard]] std::string getCommonName() const
		{
			return get({ 2, 5, 4, 3 });
		}

		[[nodiscard]] std::string getCountry() const
		{
			return get({ 2, 5, 4, 6 });
		}

		[[nodiscard]] std::string getOrganisationName() const
		{
			return get({ 2, 5, 4, 10 });
		}
	};
}
