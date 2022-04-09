#include "asn1_identifier.hpp"

#include "string_writer.hpp"

namespace soup
{
	std::string asn1_identifier::toDer() const
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
			string_writer w{};
			w.om(type);
			ret.append(w.str);
		}
		return ret;
	}
}
