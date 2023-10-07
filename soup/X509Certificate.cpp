#include "X509Certificate.hpp"

#include <cstring> // memcmp

#include "Asn1Type.hpp"
#include "IpAddr.hpp"
#include "joaat.hpp"
#include "sha1.hpp"
#include "sha256.hpp"
#include "string.hpp"

namespace soup
{
	bool X509Certificate::fromDer(const std::string& str)
	{
		return load(Asn1Sequence::fromDer(str));
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
			sig = cert.getString(2);

			hash = joaat::hash(cert.at(0).data);

			auto pubInfo = tbsCert.getSeq(6);
			auto pubType = pubInfo.getSeq(0);
			auto pubCrypto = pubType.getOid(0);

			std::string pubKeyStr = pubInfo.getString(1);
			pubKeyStr.erase(0, 1);

			if (pubCrypto == Oid::RSA_ENCRYPTION)
			{
				is_ec = false;

				auto pubKey = Asn1Sequence::fromDer(pubKeyStr);
				setRsaPublicKey(
					pubKey.getInt(0),
					pubKey.getInt(1)
				);
			}
			else if (pubCrypto == Oid::EC_PUBLIC_KEY)
			{
				is_ec = true;

				auto pubCurve = pubType.getOid(1);
				if (pubCurve == Oid::PRIME256V1)
				{
					curve = &EccCurve::secp256r1();
				}
				else if (pubCurve == Oid::ANSIP384R1)
				{
					curve = &EccCurve::secp384r1();
				}
				else
				{
					curve = nullptr;
				}

				if (curve)
				{
					key = curve->decodePoint(pubKeyStr);
				}
			}
			else
			{
				SOUP_ASSERT_UNREACHABLE;
			}

			issuer = readRelativeDistinguishedName(tbsCert.getSeq(3));
			subject = readRelativeDistinguishedName(tbsCert.getSeq(5));

			Asn1Sequence validityPeriod = tbsCert.getSeq(4);
			valid_from = validityPeriod.getUtctime(0);
			valid_to = validityPeriod.getUtctime(1);

			Asn1Sequence extensions = tbsCert.getSeq(7).getSeq(0);
			for (size_t i = 0; i != extensions.countChildren(); ++i)
			{
				Asn1Sequence ext = extensions.getSeq(i);
				if (ext.getOid(0) == Oid::SUBJECT_ALT_NAME)
				{
					size_t data_idx = ((ext.at(1).identifier.type == Asn1Type::_BOOLEAN) ? 2 : 1);
					// RFC 2459, page 33
					Asn1Sequence data = Asn1Sequence::fromDer(ext.getString(data_idx));
					for (size_t j = 0; j != data.countChildren(); ++j)
					{
						if (data.getChildType(j).type == 2) // DNS Name
						{
							subject_alt_names.emplace_back(std::move(data.getString(j)));
						}
						else if (data.getChildType(j).type == 7) // IP Address
						{
							if (data.getString(j).size() == 4)
							{
								subject_alt_names.emplace_back(IpAddr(*(network_u32_t*)data.getString(j).data()).toString4());
							}
							else if (data.getString(j).size() == 16)
							{
								subject_alt_names.emplace_back(IpAddr((uint8_t*)data.getString(j).data()).toString6());
							}
						}
					}
				}
			}

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
		for (size_t i = 0; i != seq.countChildren(); ++i)
		{
			auto kv = seq.getSeq(i).getSeq(0);
			ret.emplace_back(kv.getOid(0), kv.getString(1));
		}
		return ret;
	}

	bool X509Certificate::isRsa() const noexcept
	{
		return !is_ec;
	}

	bool X509Certificate::isEc() const noexcept
	{
		return is_ec;
	}

	void X509Certificate::setRsaPublicKey(Bigint&& n, Bigint&& e)
	{
		key = EccPoint(std::move(n), std::move(e));
	}

	RsaPublicKey X509Certificate::getRsaPublicKey() const
	{
		return RsaPublicKey(key.getX(), key.getY());
	}

	bool X509Certificate::canBeVerified() const noexcept
	{
		switch (sig_type)
		{
		case RSA_WITH_SHA1:
		case RSA_WITH_SHA256:
		case ECDSA_WITH_SHA256:
			return true;

		default:;
		}
		return false;
	}

	bool X509Certificate::verify(const X509Certificate& issuer) const
	{
		switch (sig_type)
		{
		case RSA_WITH_SHA1:
			if (!issuer.isRsa())
			{
				return false;
			}
			return issuer.getRsaPublicKey().verify<soup::sha1>(tbsCertDer, Bigint::fromBinary(sig));

		case RSA_WITH_SHA256:
			if (!issuer.isRsa())
			{
				return false;
			}
			return issuer.getRsaPublicKey().verify<soup::sha256>(tbsCertDer, Bigint::fromBinary(sig));

		case ECDSA_WITH_SHA256:
			if (!issuer.isEc()
				|| !issuer.curve
				)
			{
				return false;
			}
			{
				auto seq = Asn1Sequence(sig.substr(1)).getSeq(0);
				auto r = seq.getInt(0);
				auto s = seq.getInt(1);
				return issuer.curve->verify(issuer.key, sha256::hash(tbsCertDer), r, s);
			}

			// TODO: Implement SHA384 & SHA512
		case RSA_WITH_SHA384: return issuer.isRsa();
		case RSA_WITH_SHA512: return issuer.isRsa();
		case ECDSA_WITH_SHA384: return issuer.isEc();

		case UNK_WITH_UNK:;
		}
		return false;
	}

	bool X509Certificate::isValidForDomain(const std::string& domain) const
	{
		if (matchDomain(domain, subject.getCommonName()))
		{
			return true;
		}
		for (const auto& name : subject_alt_names)
		{
			if (matchDomain(domain, name))
			{
				return true;
			}
		}
		return false;
	}

	bool X509Certificate::matchDomain(const std::string& domain, const std::string& name)
	{
		auto domain_parts = string::explode(domain, '.');
		auto name_parts = string::explode(name, '.');
		if (domain_parts.size() != name_parts.size())
		{
			return false;
		}
		for (size_t i = 0; i != domain_parts.size(); ++i)
		{
			if (name_parts.at(i) != "*"
				&& name_parts.at(i) != domain_parts.at(i)
				)
			{
				return false;
			}
		}
		return true;
	}
}
