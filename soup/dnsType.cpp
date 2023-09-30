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
		case DNS_MX: return "MX";
		case DNS_SRV: return "SRV";
		case DNS_NS: return "NS";
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
		if (str == "MX") return DNS_MX;
		if (str == "SRV") return DNS_SRV;
		if (str == "NS") return DNS_NS;

		std::string msg = "Unknown dnsType: ";
		msg.append(str);
		SOUP_THROW(Exception(std::move(msg)));
	}
}
