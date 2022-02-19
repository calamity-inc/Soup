#pragma once

#include "asn1_sequence.hpp"
#include "rsa.hpp"
#include "x509_relative_distinguished_name.hpp"

namespace soup
{
	class x509_certificate
	{
	public:
		x509_relative_distinguished_name issuer;
		x509_relative_distinguished_name subject;
		rsa::key_public key;

		bool fromBinary(const std::string& str);
		bool fromBinary(std::istream& s);
		bool load(const asn1_sequence& cert);
	protected:
		[[nodiscard]] static x509_relative_distinguished_name readRelativeDistinguishedName(const asn1_sequence& seq);
	};
}
