#pragma once

#include <istream>
#include <unordered_map>

#include "rsa.hpp"

namespace soup
{
	struct TrustStore
	{
		std::unordered_map<std::string, RsaPublicKey> data{};

		void loadCaCerts(std::istream& is); // designed for contents of cacert.pem, which can be downloaded from https://curl.se/docs/caextract.html
		void addCa(std::string&& common_name, std::string&& pem);
		void addCa(std::string&& common_name, RsaPublicKey&& key);

		[[nodiscard]] const RsaPublicKey* findCommonName(const std::string& cn) const;
	};
}
