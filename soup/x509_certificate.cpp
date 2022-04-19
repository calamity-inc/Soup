#include "x509_certificate.hpp"

#include <cstring> // memcmp

#include "sha256.hpp"

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
		try
		{
			auto tbsCert = cert.getSeq(0);
			tbsCertDer = tbsCert.toDer();
			sig = cert.getInt(2);

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
		catch (const std::out_of_range&)
		{
		}
		return false;
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

	bool x509_certificate::isRsa() const noexcept
	{
		return !key.n.isZero();
	}

	bool x509_certificate::isEc() const noexcept
	{
		return !isRsa();
	}

	bool x509_certificate::verify(const x509_certificate& issuer) const
	{
		return verify(issuer.key);
	}

	bool x509_certificate::verify(const rsa::key_public& issuer) const
	{
		// Assuming cert[1] is sequence of OID 1.2.840.113549.1.1.11 (rsa with sha256)
		return issuer.verify<soup::sha256>(tbsCertDer, sig);
	}
}
