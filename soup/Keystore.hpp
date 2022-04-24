#pragma once

#include <istream>
#include <unordered_map>

#include "rsa.hpp"

namespace soup
{
	struct Keystore
	{
		std::unordered_map<std::string, rsa::PublicKey> data{};

		void loadCaCerts(std::istream& is); // designed for contents of cacert.pem, which can be downloaded from https://curl.se/docs/caextract.html
		void addCa(std::string&& common_name, std::string&& pem);
		void addCa(std::string&& common_name, rsa::PublicKey&& key);

		[[nodiscard]] const rsa::PublicKey* findCommonName(const std::string& cn) const;
	};
}
