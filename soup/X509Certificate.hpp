#pragma once

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
		rsa::PublicKey key;

		bool fromBinary(const std::string& str);
		bool fromBinary(std::istream& s);
		bool load(const Asn1Sequence& cert);
	protected:
		[[nodiscard]] static X509RelativeDistinguishedName readRelativeDistinguishedName(const Asn1Sequence& seq);

	public:
		[[nodiscard]] bool isRsa() const noexcept;
		[[nodiscard]] bool isEc() const noexcept;

		[[nodiscard]] bool verify(const X509Certificate& issuer) const;
		[[nodiscard]] bool verify(const rsa::PublicKey& issuer) const;
	};
}
