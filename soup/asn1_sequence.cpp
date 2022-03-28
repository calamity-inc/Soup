#include "asn1_sequence.hpp"

#include <sstream>

#include "asn1_type.hpp"
#include "bigint.hpp"
#include "oid.hpp"
#include "stream.hpp"
#include "string.hpp"

namespace soup
{
	asn1_sequence::asn1_sequence()
		: std::vector<asn1_element>()
	{
	}

	asn1_sequence::asn1_sequence(std::string data)
		: asn1_sequence()
	{
		std::istringstream s{ std::move(data) };
		while (s.peek() != EOF)
		{
			auto id = readIdentifier(s);
			auto len = readLength(s);
			std::string buf(len, '\0');
			s.read(buf.data(), len);
			emplace_back(asn1_element{ std::move(id), std::move(buf) });
		}
	}

	asn1_sequence asn1_sequence::fromBinary(const std::string& str)
	{
		std::istringstream s{ str };
		return fromBinary(s);
	}

	asn1_sequence asn1_sequence::fromBinary(std::istream& s)
	{
		readIdentifier(s);
		auto len = readLength(s);
		std::string buf(len, '\0');
		s.read(buf.data(), len);
		return asn1_sequence{ std::move(buf) };
	}

	size_t asn1_sequence::countChildren() const
	{
		return size();
	}

	const asn1_identifier& asn1_sequence::getChildType(const size_t child_idx) const
	{
		return at(child_idx).identifier;
	}

	const std::string& asn1_sequence::getString(const size_t child_idx) const
	{
		return at(child_idx).data;
	}

	asn1_sequence asn1_sequence::getSeq(const size_t child_idx) const
	{
		return asn1_sequence{ getString(child_idx) };
	}

	bigint asn1_sequence::getInt(const size_t child_idx) const
	{
		return bigint::fromBinary(getString(child_idx));
	}

	oid asn1_sequence::getOid(const size_t child_idx) const
	{
		return oid::fromBinary(getString(child_idx));
	}

	void asn1_sequence::addInt(const bigint& val)
	{
		std::string bin = val.toBinary();
		if (bin.empty())
		{
			bin = std::string(1, '\0');
		}
		emplace_back(asn1_element{
			asn1_identifier{ 0, false, asn1_type::INTEGER },
			std::move(bin)
		});
	}

	std::string asn1_sequence::toDer() const
	{
		std::string ret{};
		for (const auto& c : *this)
		{
			ret.append(c.identifier.toDer());
			ret.append(encodeLength(c.data.size()));
			ret.append(c.data);
		}
		ret.insert(0, encodeLength(ret.size()));
		ret.insert(0, asn1_identifier{ 0, true, asn1_type::SEQUENCE }.toDer());
		return ret;
	}

	std::string asn1_sequence::toString(const std::string& prefix) const
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

				case asn1_type::_BOOLEAN:
					ret.append("BOOLEAN");
					break;

				case asn1_type::INTEGER:
					ret.append("INTEGER");
					break;

				case asn1_type::BITSTRING:
					ret.append("BIT STRING");
					break;

				case asn1_type::STRING_OCTET:
					ret.append("OCTET STRING");
					break;

				case asn1_type::_NULL:
					ret.append("NULL");
					break;

				case asn1_type::OID:
					ret.append("OID");
					break;

				case asn1_type::SEQUENCE:
					ret.append("SEQUENCE");
					break;

				case asn1_type::SET:
					ret.append("SET");
					break;

				case asn1_type::STRING_PRINTABLE:
					ret.append("PrintableString");
					break;

				case asn1_type::STRING_IA5:
					ret.append("IA5String");
					break;

				case asn1_type::UTCTIME:
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
				&& c.identifier.type != asn1_type::SEQUENCE
				&& c.identifier.type != asn1_type::SET
				)
			{
				ret.push_back(' ');
				switch (c.identifier.type)
				{
				default:
					ret.append(string::bin2hex(c.data));
					break;

				case asn1_type::INTEGER:
				case asn1_type::BITSTRING:
					ret.append(bigint::fromBinary(c.data).toString());
					break;

				case asn1_type::OID:
					ret.append(oid::fromBinary(c.data).toString());
					break;

				case asn1_type::STRING_PRINTABLE:
				case asn1_type::STRING_IA5:
				case asn1_type::UTCTIME: // yyMMddHHmmssZ
					ret.append(c.data);
					break;
				}
			}
			else
			{
				ret.push_back('\n');
				std::string rp = prefix;
				rp.push_back('\t');
				ret.append(asn1_sequence(c.data).toString(rp));
			}
		}
		if (!ret.empty())
		{
			ret.erase(0, 1);
		}
		return ret;
	}

	asn1_identifier asn1_sequence::readIdentifier(std::istream& s)
	{
		asn1_identifier ret{};
		auto first = (uint8_t)s.get();
		ret.m_class = (first >> 6);
		ret.constructed = (first >> 5) & 1;
		ret.type = (first & 0b11111);
		if (ret.type > 30)
		{
			ret.type = stream::readOmInt<uint32_t>(s);
		}
		return ret;
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

	std::string asn1_sequence::encodeLength(size_t len)
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
