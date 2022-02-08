#pragma once

#include "bigint.hpp"

namespace soup
{
	struct rsa
	{
		struct key
		{
			bigint n;
			bigint e;

			key(bigint n, bigint e)
				: n(std::move(n)), e(std::move(e))
			{
			}

			[[nodiscard]] size_t getMaxUnpaddedMessageBytes() const;
			[[nodiscard]] size_t getMaxPkcs1MessageBytes() const;

			[[nodiscard]] bigint modPow(const bigint& x) const;

			[[nodiscard]] bigint encryptUnpadded(const std::string& msg) const; // not secure if `msg` is not unique

			[[nodiscard]] std::string decryptPkcs1(const bigint& enc) const;
			[[nodiscard]] std::string decryptUnpadded(const bigint& enc) const;

			[[nodiscard]] bigint sign(const bigint& hash) const;
			[[nodiscard]] bool verify(const bigint& hash, const bigint& sig) const;
		};

		struct key_public : public key
		{
#if SOUP_PLATFORM_BITS > 32
			static bigint::chunk_t e_pref;
#else
			static bigint e_pref;
#endif

			using key::key;

			[[nodiscard]] bigint encryptPkcs1(std::string msg) const;
		};

		struct key_private : public key
		{
			using key::key;

			[[nodiscard]] key_public derivePublic() const; // this is a minimal representation, so public key derivation assumes that e = e_pref, which is true unless your keypair is 21-bit or less

			[[nodiscard]] bigint encryptPkcs1(std::string msg) const; // not secure if `msg` is not unique
		};

		struct keypair
		{
			bigint n;
			bigint e;
			bigint d;

			void random(int bits);
			void fromPrimes(const bigint& p, const bigint& q);

			[[nodiscard]] key_public getPublic() const;
			[[nodiscard]] key_private getPrivate() const;
		};
	};
}
