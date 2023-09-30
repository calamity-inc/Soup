#include "X509Certificate.hpp"

#include <cstring> // memcmp

#include "joaat.hpp"
#include "sha1.hpp"
#include "sha256.hpp"

namespace soup
{
	bool X509Certificate::fromDer(const std::string& str)
	{
		return load(Asn1Sequence::fromDer(str));
	}

	bool X509Certificate::fromDer(std::istream& s)
	{
		return load(Asn1Sequence::fromDer(s));
	}

	bool X509Certificate::load(const Asn1Sequence& cert)
	{
#if SOUP_EXCEPTIONS
		try
#endif
		{
			auto tbsCert = cert.getSeq(0);
			switch (joaat::hash(cert.getSeq(1).getOid(0).toString()))
			{
			case joaat::hash("1.2.840.113549.1.1.5"):
				sig_type = RSA_WITH_SHA1;
				break;

			case joaat::hash("1.2.840.113549.1.1.11"):
				sig_type = RSA_WITH_SHA256;
				break;

			case joaat::hash("1.2.840.113549.1.1.12"):
				sig_type = RSA_WITH_SHA384;
				break;

			case joaat::hash("1.2.840.113549.1.1.13"):
				sig_type = RSA_WITH_SHA512;
				break;

			case joaat::hash("1.2.840.10045.4.3.2"):
				sig_type = ECDSA_WITH_SHA256;
				break;

			case joaat::hash("1.2.840.10045.4.3.3"):
				sig_type = ECDSA_WITH_SHA384;
				break;
			}
			tbsCertDer = tbsCert.toDer();
			sig = cert.getInt(2);

			hash = joaat::hash(cert.at(0).data);

			auto pubInfo = tbsCert.getSeq(6);
			auto oid_bin = pubInfo.getString(0);

			const unsigned char rsa_oid[] = {
				0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, // OID 1.2.840.113549.1.1.1
				0x05, 0x00 // NULL
			};
			if (oid_bin.size() == sizeof(rsa_oid) && memcmp(oid_bin.data(), rsa_oid, sizeof(rsa_oid)) == 0)
			{
				std::string pubKeyStr = pubInfo.getString(1);
				while (pubKeyStr.at(0) == '\0')
				{
					pubKeyStr.erase(0, 1);
				}
				auto pubKey = Asn1Sequence::fromDer(pubKeyStr);
				key.n = pubKey.getInt(0);
				key.e = pubKey.getInt(1);
			}

			issuer = readRelativeDistinguishedName(tbsCert.getSeq(3));
			subject = readRelativeDistinguishedName(tbsCert.getSeq(5));

			Asn1Sequence validityPeriod = tbsCert.getSeq(4);
			valid_from = validityPeriod.getUtctime(0);
			valid_to = validityPeriod.getUtctime(1);

			return true;
		}
#if SOUP_EXCEPTIONS
		catch (const std::out_of_range&)
		{
		}
#endif
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

	bool X509Certificate::canBeVerified() const noexcept
	{
		switch (sig_type)
		{
		case RSA_WITH_SHA1:
		case RSA_WITH_SHA256:
			return true;

		default:;
		}
		return false;
	}

	bool X509Certificate::verify(const X509Certificate& issuer) const
	{
		return verify(issuer.key);
	}

	bool X509Certificate::verify(const RsaPublicKey& issuer) const
	{
		switch (sig_type)
		{
		case RSA_WITH_SHA1:
			return issuer.verify<soup::sha1>(tbsCertDer, sig);

		case RSA_WITH_SHA256:
			return issuer.verify<soup::sha256>(tbsCertDer, sig);

		case RSA_WITH_SHA384:
		case RSA_WITH_SHA512:
		case ECDSA_WITH_SHA256:
		case ECDSA_WITH_SHA384:
			return true; // TODO

		case UNK_WITH_UNK:;
		}
		return false;
	}
}
