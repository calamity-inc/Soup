#include "pem.hpp"

#include "base64.hpp"
#include "string.hpp"

namespace soup
{
	std::string pem::decode(std::string in)
	{
		string::erase<std::string>(in, "-----BEGIN CERTIFICATE-----");
		string::erase<std::string>(in, "-----END CERTIFICATE-----");
		string::erase<std::string>(in, "\r");
		string::erase<std::string>(in, "\n");
		string::erase<std::string>(in, "\t");
		string::erase<std::string>(in, " ");
		return base64::decode(in);
	}
}
