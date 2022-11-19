#include "dnsType.hpp"

#include "Exception.hpp"

namespace soup
{
	std::string dnsTypeToString(dnsType type)
	{
		switch (type)
		{
		case DNS_A: return "A";
		case DNS_AAAA: return "AAAA";
		case DNS_CNAME: return "CNAME";
		case DNS_PTR: return "PTR";
		case DNS_TXT: return "TXT";
		default:;
		}
		return std::to_string(type);
	}

	dnsType dnsTypeFromString(const std::string& str)
	{
		if (str == "A") return DNS_A;
		if (str == "AAAA") return DNS_AAAA;
		if (str == "CNAME") return DNS_CNAME;
		if (str == "PTR") return DNS_PTR;
		if (str == "TXT") return DNS_TXT;

		std::string msg = "Unknown dnsType: ";
		msg.append(str);
		throw Exception(std::move(msg));
	}
}
