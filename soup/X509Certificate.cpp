#include "X509Certificate.hpp"

#include <cstring> // memcmp

#include "sha256.hpp"

namespace soup
{
	bool X509Certificate::fromBinary(const std::string& str)
	{
		return load(Asn1Sequence::fromBinary(str));
	}

	bool X509Certificate::fromBinary(std::istream& s)
	{
		return load(Asn1Sequence::fromBinary(s));
	}

	bool X509Certificate::load(const Asn1Sequence& cert)
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
				auto pubKey = Asn1Sequence::fromBinary(pubKeyStr);
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

	X509RelativeDistinguishedName X509Certificate::readRelativeDistinguishedName(const Asn1Sequence& seq)
	{
		X509RelativeDistinguishedName ret{};
		for (auto i = 0; i != seq.countChildren(); ++i)
		{
			auto kv = seq.getSeq(i).getSeq(0);
			ret.emplace_back(kv.getOid(0), kv.getString(1));
		}
		return ret;
	}

	bool X509Certificate::isRsa() const noexcept
	{
		return !key.n.isZero();
	}

	bool X509Certificate::isEc() const noexcept
	{
		return !isRsa();
	}

	bool X509Certificate::verify(const X509Certificate& issuer) const
	{
		return verify(issuer.key);
	}

	bool X509Certificate::verify(const RsaPublicKey& issuer) const
	{
		// Assuming cert[1] is sequence of OID 1.2.840.113549.1.1.11 (rsa with sha256)
		return issuer.verify<soup::sha256>(tbsCertDer, sig);
	}
}
