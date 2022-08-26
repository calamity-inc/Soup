#include "dnsType.hpp"

namespace soup
{
	std::string dnsTypeToString(dnsType type)
	{
		switch (type)
		{
		case DNS_A: return "A";
		case DNS_AAAA: return "AAAA";
		case DNS_CNAME: return "CNAME";
		default:;
		}
		return std::to_string(type);
	}
}
