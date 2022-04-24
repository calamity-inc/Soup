#include "Asn1Sequence.hpp"

#include <sstream>

#include "Asn1Type.hpp"
#include "Bigint.hpp"
#include "IstreamReader.hpp"
#include "Oid.hpp"
#include "string.hpp"

namespace soup
{
	Asn1Sequence::Asn1Sequence()
		: std::vector<Asn1Element>()
	{
	}

	Asn1Sequence::Asn1Sequence(std::string data)
		: Asn1Sequence()
	{
		std::istringstream s{ std::move(data) };
		while (s.peek() != EOF)
		{
			auto id = readIdentifier(s);
			auto len = readLength(s);
			std::string buf(len, '\0');
			s.read(buf.data(), len);
			emplace_back(Asn1Element{ std::move(id), std::move(buf) });
		}
	}

	Asn1Sequence Asn1Sequence::fromBinary(const std::string& str)
	{
		std::istringstream s{ str };
		return fromBinary(s);
	}

	Asn1Sequence Asn1Sequence::fromBinary(std::istream& s)
	{
		readIdentifier(s);
		auto len = readLength(s);
		std::string buf(len, '\0');
		s.read(buf.data(), len);
		return Asn1Sequence{ std::move(buf) };
	}

	size_t Asn1Sequence::countChildren() const
	{
		return size();
	}

	const Asn1Identifier& Asn1Sequence::getChildType(const size_t child_idx) const
	{
		return at(child_idx).identifier;
	}

	const std::string& Asn1Sequence::getString(const size_t child_idx) const
	{
		return at(child_idx).data;
	}

	Asn1Sequence Asn1Sequence::getSeq(const size_t child_idx) const
	{
		return Asn1Sequence{ getString(child_idx) };
	}

	Bigint Asn1Sequence::getInt(const size_t child_idx) const
	{
		return Bigint::fromBinary(getString(child_idx));
	}

	Oid Asn1Sequence::getOid(const size_t child_idx) const
	{
		return Oid::fromBinary(getString(child_idx));
	}

	void Asn1Sequence::addInt(const Bigint& val)
	{
		std::string bin = val.toBinary();
		if (bin.empty())
		{
			bin = std::string(1, '\0');
		}
		emplace_back(Asn1Element{
			Asn1Identifier{ 0, false, Asn1Type::INTEGER },
			std::move(bin)
		});
	}

	std::string Asn1Sequence::toDer() const
	{
		std::string ret{};
		for (const auto& c : *this)
		{
			ret.append(c.identifier.toDer());
			ret.append(encodeLength(c.data.size()));
			ret.append(c.data);
		}
		ret.insert(0, encodeLength(ret.size()));
		ret.insert(0, Asn1Identifier{ 0, true, Asn1Type::SEQUENCE }.toDer());
		return ret;
	}

	std::string Asn1Sequence::toString(const std::string& prefix) const
	{
		std::string ret{};
		size_t i = 0;
		for (const auto& c : *this)
		{
			ret.push_back('\n');
			ret.append(prefix);
			ret.push_back('[');
			ret.append(std::to_string(i++));
			ret.append("] ");
			ret.append(c.identifier.constructed ? "Constructed " : "Primitive ");
			if (c.identifier.m_class == 0)
			{
				switch (c.identifier.type)
				{
				default:
					ret.append(std::to_string(c.identifier.type));
					break;

				case Asn1Type::_BOOLEAN:
					ret.append("BOOLEAN");
					break;

				case Asn1Type::INTEGER:
					ret.append("INTEGER");
					break;

				case Asn1Type::BITSTRING:
					ret.append("BIT STRING");
					break;

				case Asn1Type::STRING_OCTET:
					ret.append("OCTET STRING");
					break;

				case Asn1Type::_NULL:
					ret.append("NULL");
					break;

				case Asn1Type::OID:
					ret.append("OID");
					break;

				case Asn1Type::SEQUENCE:
					ret.append("SEQUENCE");
					break;

				case Asn1Type::SET:
					ret.append("SET");
					break;

				case Asn1Type::STRING_PRINTABLE:
					ret.append("PrintableString");
					break;

				case Asn1Type::STRING_IA5:
					ret.append("IA5String");
					break;

				case Asn1Type::UTCTIME:
					ret.append("UTCTime");
					break;
				}
			}
			else
			{
				switch (c.identifier.m_class)
				{
				case 1:
					ret.append("Application-specific ");
					break;

				case 2:
					ret.append("Context-specific ");
					break;

				case 3:
					ret.append("Private ");
					break;
				}
				ret.append(std::to_string(c.identifier.type));
			}
			if (c.data.empty())
			{
				continue;
			}
			ret.push_back(':');
			if (c.identifier.m_class == 0
				&& c.identifier.type != Asn1Type::SEQUENCE
				&& c.identifier.type != Asn1Type::SET
				)
			{
				ret.push_back(' ');
				switch (c.identifier.type)
				{
				default:
					ret.append(string::bin2hex(c.data));
					break;

				case Asn1Type::INTEGER:
				case Asn1Type::BITSTRING:
					ret.append(Bigint::fromBinary(c.data).toString());
					break;

				case Asn1Type::OID:
					ret.append(Oid::fromBinary(c.data).toString());
					break;

				case Asn1Type::STRING_PRINTABLE:
				case Asn1Type::STRING_IA5:
				case Asn1Type::UTCTIME: // yyMMddHHmmssZ
					ret.append(c.data);
					break;
				}
			}
			else
			{
				ret.push_back('\n');
				std::string rp = prefix;
				rp.push_back('\t');
				ret.append(Asn1Sequence(c.data).toString(rp));
			}
		}
		if (!ret.empty())
		{
			ret.erase(0, 1);
		}
		return ret;
	}

	Asn1Identifier Asn1Sequence::readIdentifier(std::istream& s)
	{
		Asn1Identifier ret{};
		auto first = (uint8_t)s.get();
		ret.m_class = (first >> 6);
		ret.constructed = (first >> 5) & 1;
		ret.type = (first & 0b11111);
		if (ret.type > 30)
		{
			IstreamReader r(&s);
			r.om<uint32_t>(ret.type);
		}
		return ret;
	}

	size_t Asn1Sequence::readLength(std::istream& s)
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
				auto r = s.get();
				if (r == EOF)
				{
					break;
				}
				length <<= 8;
				length |= (uint8_t)r;
			}
		}
		return length;
	}

	std::string Asn1Sequence::encodeLength(size_t len)
	{
		std::string ret{};
		if (len <= 0x7F)
		{
			ret.push_back(len);
		}
		else
		{
			do
			{
				ret.insert(0, 1, (char)(unsigned char)len);
			} while (len >>= 8, len != 0);
			ret.insert(0, 1, ret.size() | 0x80);
		}
		return ret;
	}
}
