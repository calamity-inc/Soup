#include "pem.hpp"

#include "base64.hpp"
#include "string.hpp"

namespace soup
{
	std::string pem::encode(const std::string& label, const std::string& bin)
	{
		std::string res = "-----BEGIN ";
		res.append(label);
		res.append("-----");
		auto b64 = base64::encode(bin);
		while (!b64.empty())
		{
			res.push_back('\n');
			res.append(b64.substr(0, 64));
			b64.erase(0, 64);
		}
		res.append("\n-----END ");
		res.append(label);
		res.append("-----");
		return res;
	}

	std::string pem::decode(std::string in)
	{
		string::erase<std::string>(in, "-----BEGIN CERTIFICATE-----");
		string::erase<std::string>(in, "-----END CERTIFICATE-----");
		string::erase<std::string>(in, "-----BEGIN RSA PRIVATE KEY-----");
		string::erase<std::string>(in, "-----END RSA PRIVATE KEY-----");
		string::erase<std::string>(in, "\r");
		string::erase<std::string>(in, "\n");
		string::erase<std::string>(in, "\t");
		string::erase<std::string>(in, " ");
		return base64::decode(in);
	}
}
