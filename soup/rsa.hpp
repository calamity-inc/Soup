#pragma once

#include "fwd.hpp"

#include "bigint.hpp"

namespace soup
{
	struct rsa
	{
		struct mod
		{
			bigint n{};

			mod() = default;

			mod(const bigint& n)
				: n(n)
			{
			}

			[[nodiscard]] size_t getMaxUnpaddedMessageBytes() const;
			[[nodiscard]] size_t getMaxPkcs1MessageBytes() const;

			bool padPublic(std::string& str) const; // non-deterministic
			bool padPrivate(std::string& str) const; // deterministic

			template <typename CryptoHashAlgo>
			bool padHash(std::string& bin) const // deterministic
			{
				return CryptoHashAlgo::prependId(bin)
					&& padPrivate(bin)
					;
			}

			static bool unpad(std::string& str);
		};

		template <typename T>
		struct key : public mod
		{
			using mod::mod;

			[[nodiscard]] bigint encryptUnpadded(const std::string& msg) const // deterministic
			{
				return reinterpret_cast<const T*>(this)->modPow(bigint::fromBinary(msg));
			}

			[[nodiscard]] std::string decryptUnpadded(const bigint& enc) const
			{
				return reinterpret_cast<const T*>(this)->modPow(enc).toBinary();
			}

			[[nodiscard]] std::string decryptPkcs1(const bigint& enc) const
			{
				auto msg = decryptUnpadded(enc);
				unpad(msg);
				return msg;
			}
		};

		template <typename T>
		struct key_public_base : public key<T>
		{
			bigint e{};

			key_public_base() = default;

			key_public_base(const bigint& n, const bigint& e)
				: key<T>(n), e(e)
			{
			}

			[[nodiscard]] bigint encryptPkcs1(std::string msg) const // non-deterministic
			{
				key<T>::padPublic(msg);
				return key<T>::encryptUnpadded(msg);
			}

			template <typename CryptoHashAlgo>
			[[nodiscard]] bool verify(const std::string& msg, const bigint& sig) const
			{
				auto hash_bin = CryptoHashAlgo::hash(msg);
				return key<T>::template padHash<CryptoHashAlgo>(hash_bin)
					&& key<T>::decryptUnpadded(sig) == hash_bin;
			}
		};

		struct key_public : public key_public_base<key_public>
		{
			static bigint e_pref;

			key_public() = default;
			key_public(const bigint& n);
			key_public(const bigint& n, const bigint& e);

			[[nodiscard]] bigint modPow(const bigint& x) const;
		};

		struct key_montgomery_data
		{
			size_t re{};
			bigint r{};
			bigint n_mod_mul_inv{};
			bigint r_mod_mul_inv{};
			bigint one_mont{};

			key_montgomery_data() = default;
			key_montgomery_data(const bigint& n, const bigint& e);

			[[nodiscard]] bigint modPow(const bigint& n, const bigint& e, const bigint& x) const;
		};

		/*
		* In the case of an 1024-bit rsa public key, using a long-lived instance takes ~134ms, but performs operations in ~2ms, compared to
		* ~12ms using a short-lived instance. From these numbers, we can estimate that a long-lived instance is the right choice for rsa public
		* keys that are (expected to be) used more than 13 times.
		*/
		struct key_public_longlived : public key_public_base<key_public_longlived>
		{
			key_montgomery_data mont_data;

			key_public_longlived() = default;
			key_public_longlived(const bigint& n);
			key_public_longlived(const bigint& n, const bigint& e);

			[[nodiscard]] bigint modPow(const bigint& x) const;
		};

		struct key_private : public key<key_private>
		{
			bigint p;
			bigint q;
			bigint dp;
			bigint dq;
			bigint qinv;

			key_montgomery_data p_mont_data;
			key_montgomery_data q_mont_data;

			key_private() = default;
			key_private(const bigint& n, const bigint& p, const bigint& q, const bigint& dp, const bigint& dq, const bigint& qinv);

			[[nodiscard]] static key_private fromBinary(const std::string bin);
			[[nodiscard]] static key_private fromAsn1(const asn1_sequence& seq);

			template <typename CryptoHashAlgo>
			[[nodiscard]] bigint sign(const std::string& msg) const // deterministic
			{
				return encryptPkcs1(CryptoHashAlgo::hashWithId(msg));
			}

			[[nodiscard]] bigint encryptPkcs1(std::string msg) const; // deterministic

			[[nodiscard]] key_public derivePublic() const; // assumes that e = e_pref, which is true unless your keypair is 21-bit or less.

			[[nodiscard]] asn1_sequence toAsn1() const; // as per PKCS#1. assumes that e = e_pref, which is true unless your keypair is 21-bit or less.
			[[nodiscard]] std::string toPem() const; // assumes that e = e_pref, which is true unless your keypair is 21-bit or less.

			[[nodiscard]] bigint modPow(const bigint& x) const;
			[[nodiscard]] bigint getE() const; // returns public exponent. assumes that e = e_pref, which is true unless your keypair is 21-bit or less.
			[[nodiscard]] bigint getD() const; // returns private exponent. assumes that e = e_pref, which is true unless your keypair is 21-bit or less.
		};

		struct keypair : public mod
		{
			bigint p;
			bigint q;
			bigint e;
			bigint dp;
			bigint dq;
			bigint qinv;

			keypair() = default;
			keypair(bigint&& _p, bigint&& _q);

			[[nodiscard]] static keypair random(unsigned int bits);

			[[nodiscard]] key_public getPublic() const;
			[[nodiscard]] key_private getPrivate() const;
		};
	};
}
