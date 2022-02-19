#include "asn1_sequence.hpp"

#include <sstream>

namespace soup
{
	asn1_sequence asn1_sequence::fromBinary(const std::string& str)
	{
		std::istringstream s{ str };
		return fromBinary(s);
	}

	asn1_sequence asn1_sequence::fromBinary(std::istream& s)
	{
		skipType(s);
		auto len = readLength(s);
		std::string buf(len, '\0');
		s.read(buf.data(), len);
		return asn1_sequence{ buf };
	}

	size_t asn1_sequence::countChildren() const
	{
		std::istringstream s{ data };
		size_t c = 0;
		while (s.peek() != EOF)
		{
			skipType(s);
			s.ignore(readLength(s));
			++c;
		}
		return c;
	}

	asn1_sequence asn1_sequence::getSeq(const size_t child_idx) const
	{
		return asn1_sequence{ getString(child_idx) };
	}

	std::string asn1_sequence::getString(const size_t child_idx) const
	{
		std::istringstream s{ data };
		size_t c = 0;
		while (s.peek() != EOF)
		{
			skipType(s);
			auto len = readLength(s);
			if (c == child_idx)
			{
				std::string buf(len, '\0');
				s.read(buf.data(), len);
				return buf;
			}
			s.ignore(len);
			++c;
		}
		return {};
	}

	bigint asn1_sequence::getInt(const size_t child_idx) const
	{
		return bigint::fromBinary(getString(child_idx));
	}

	oid asn1_sequence::getOid(const size_t child_idx) const
	{
		return oid::fromBinary(getString(child_idx));
	}

	void asn1_sequence::skipType(std::istream& s)
	{
		if (auto type = ((uint8_t)s.get()) & 0b11111; type > 30)
		{
			while ((s.get() >> 7) != 1); // avoids infinite loop if EOF
		}
	}

	size_t asn1_sequence::readLength(std::istream& s)
	{
		size_t length = s.get();
		if (length == EOF)
		{
			return 0;
		}
		if (length & 0x80)
		{
			auto length_bytes = (length & 0x7F);
			length = 0;
			for (auto i = 0; i != length_bytes; ++i)
			{
				length <<= 8;
				length |= (uint8_t)s.get();
			}
		}
		return length;
	}
}
