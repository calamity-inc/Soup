#include "Asn1Identifier.hpp"

#include "StringWriter.hpp"

namespace soup
{
	std::string Asn1Identifier::toDer() const
	{
		std::string ret{};
		uint8_t first = (((m_class << 1) | constructed) << 5);
		if (type <= 30)
		{
			first |= type;
			ret.push_back(first);
		}
		else
		{
			first |= 31;
			ret.push_back(first);
			StringWriter w{};
			w.om(type);
			ret.append(w.str);
		}
		return ret;
	}
}
