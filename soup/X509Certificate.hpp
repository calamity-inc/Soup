#pragma once

#include <ctime>

#include "Asn1Sequence.hpp"
#include "rsa.hpp"
#include "X509RelativeDistinguishedName.hpp"

namespace soup
{
	class X509Certificate
	{
	protected:
		enum SigType : uint8_t
		{
			UNK_WITH_UNK,
			RSA_WITH_SHA1,
			RSA_WITH_SHA256,
			RSA_WITH_SHA384,
			RSA_WITH_SHA512,
			ECDSA_WITH_SHA256,
			ECDSA_WITH_SHA384,
		};

		std::string tbsCertDer;
		SigType sig_type;
		Bigint sig;

	public:
		uint32_t hash;
		X509RelativeDistinguishedName issuer;
		X509RelativeDistinguishedName subject;
		RsaPublicKey key;
		std::time_t valid_from;
		std::time_t valid_to;

		bool fromDer(const std::string& str);
		bool fromDer(std::istream& s);
		bool load(const Asn1Sequence& cert);
	protected:
		[[nodiscard]] static X509RelativeDistinguishedName readRelativeDistinguishedName(const Asn1Sequence& seq);

	public:
		[[nodiscard]] bool isRsa() const noexcept;
		[[nodiscard]] bool isEc() const noexcept;
		[[nodiscard]] bool canBeVerified() const noexcept;
		[[nodiscard]] bool verify(const X509Certificate& issuer) const;
		[[nodiscard]] bool verify(const RsaPublicKey& issuer) const;
	};
}
