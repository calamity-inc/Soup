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
		std::string tbsCertDer;
		Bigint sig;

	public:
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

		[[nodiscard]] bool verify(const X509Certificate& issuer) const;
		[[nodiscard]] bool verify(const RsaPublicKey& issuer) const;
	};
}
