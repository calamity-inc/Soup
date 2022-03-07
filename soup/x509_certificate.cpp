#include "x509_certificate.hpp"

#include <cstring> // memcmp

namespace soup
{
	bool x509_certificate::fromBinary(const std::string& str)
	{
		return load(asn1_sequence::fromBinary(str));
	}

	bool x509_certificate::fromBinary(std::istream& s)
	{
		return load(asn1_sequence::fromBinary(s));
	}

	bool x509_certificate::load(const asn1_sequence& cert)
	{
		auto tbsCert = cert.getSeq(0);

		auto pubInfo = tbsCert.getSeq(6);
		auto oid_bin = pubInfo.getString(0);

		const unsigned char rsa_oid[] = { 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00 };
		if (oid_bin.size() == sizeof(rsa_oid) && memcmp(oid_bin.data(), rsa_oid, sizeof(rsa_oid)) == 0)
		{
			std::string pubKeyStr = pubInfo.getString(1);
			while (pubKeyStr.at(0) == '\0')
			{
				pubKeyStr.erase(0, 1);
			}
			auto pubKey = asn1_sequence::fromBinary(pubKeyStr);
			key.n = pubKey.getInt(0);
			key.e = pubKey.getInt(1);
		}

		issuer = readRelativeDistinguishedName(tbsCert.getSeq(3));
		subject = readRelativeDistinguishedName(tbsCert.getSeq(5));

		return true;
	}

	x509_relative_distinguished_name x509_certificate::readRelativeDistinguishedName(const asn1_sequence& seq)
	{
		x509_relative_distinguished_name ret{};
		for (auto i = 0; i != seq.countChildren(); ++i)
		{
			auto kv = seq.getSeq(i).getSeq(0);
			ret.emplace_back(kv.getOid(0), kv.getString(1));
		}
		return ret;
	}
}
