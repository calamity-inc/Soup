#pragma once

#include <istream>
#include <unordered_map>

#include "rsa.hpp"

namespace soup
{
	struct keystore
	{
		std::unordered_map<std::string, rsa::key_public> data{};

		void loadCaCerts(std::istream& is); // designed for contents of cacert.pem, which can be downloaded from https://curl.se/docs/caextract.html
		void addCa(std::string&& common_name, std::string&& pem);
		void addCa(std::string&& common_name, rsa::key_public&& key);

		[[nodiscard]] const rsa::key_public* findCommonName(const std::string& cn) const;
	};
}
