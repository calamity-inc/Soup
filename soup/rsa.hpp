#pragma once

#include "bigint.hpp"

namespace soup
{
	struct rsa
	{
		struct mod
		{
			bigint n;

			mod(const bigint& n)
				: n(n)
			{
			}

			[[nodiscard]] size_t getMaxUnpaddedMessageBytes() const;
			[[nodiscard]] size_t getMaxPkcs1MessageBytes() const;
		};

		template <typename T>
		struct key : public mod
		{
			using mod::mod;

			[[nodiscard]] bigint encryptUnpadded(const std::string& msg) const // deterministic
			{
				return reinterpret_cast<const T*>(this)->modPow(bigint::fromMessage(msg));
			}

			[[nodiscard]] std::string decryptUnpadded(const bigint& enc) const
			{
				return reinterpret_cast<const T*>(this)->modPow(enc).toMessage();
			}

			[[nodiscard]] std::string decryptPkcs1(const bigint& enc) const
			{
				auto msg = decryptUnpadded(enc);
				pkcs1::unpad(msg);
				return msg;
			}
		};

		struct key_public : public key<key_public>
		{
#if SOUP_PLATFORM_BITS > 32
			static bigint::chunk_t e_pref;
#else
			static bigint e_pref;
#endif

			bigint e;

			key_public(const bigint& n, const bigint& e)
				: key(n), e(e)
			{
			}

			[[nodiscard]] bigint encryptPkcs1(std::string msg) const; // non-deterministic

			template <typename CryptoHashAlgo>
			[[nodiscard]] bool verify(const std::string& msg, const bigint& sig) const
			{
				auto hash_bin = CryptoHashAlgo::hash(msg);
				return pkcs1::padHash<CryptoHashAlgo>(hash_bin, getMaxUnpaddedMessageBytes())
					&& decryptUnpadded(sig) == hash_bin;
			}

			[[nodiscard]] bigint modPow(const bigint& x) const;
		};

		struct key_private : public key<key_private>
		{
			bigint p;
			bigint q;
			bigint dp;
			bigint dq;
			bigint qinv;

			key_private(const bigint& n, const bigint& p, const bigint& q, const bigint& dp, const bigint& dq, const bigint& qinv)
				: key(n), p(p), q(q), dp(dp), dq(dq), qinv(qinv)
			{
			}

			template <typename CryptoHashAlgo>
			[[nodiscard]] bigint sign(const std::string& msg) const // deterministic
			{
				return encryptPkcs1(CryptoHashAlgo::hashWithId(msg));
			}

			[[nodiscard]] bigint encryptPkcs1(std::string msg) const; // deterministic

			[[nodiscard]] key_public derivePublic() const; // public key derivation assumes that e = e_pref, which is true unless your keypair is 21-bit or less

			[[nodiscard]] bigint modPow(const bigint& x) const;
		};

		struct keypair : public mod
		{
			bigint p;
			bigint q;
			bigint e;
			bigint dp;
			bigint dq;
			bigint qinv;

			keypair(bigint&& _p, bigint&& _q);

			[[nodiscard]] static keypair random(unsigned int bits);

			[[nodiscard]] key_public getPublic() const;
			[[nodiscard]] key_private getPrivate() const;
		};
	};
}
