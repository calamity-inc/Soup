#pragma once

#include "fwd.hpp"

#include "Bigint.hpp"

namespace soup::rsa
{
	struct Mod
	{
		Bigint n{};

		Mod() = default;

		Mod(const Bigint& n)
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
	struct Key : public Mod
	{
		using Mod::Mod;

		[[nodiscard]] Bigint encryptUnpadded(const std::string& msg) const // deterministic
		{
			return reinterpret_cast<const T*>(this)->modPow(Bigint::fromBinary(msg));
		}

		[[nodiscard]] std::string decryptUnpadded(const Bigint& enc) const
		{
			return reinterpret_cast<const T*>(this)->modPow(enc).toBinary();
		}

		[[nodiscard]] std::string decryptPkcs1(const Bigint& enc) const
		{
			auto msg = decryptUnpadded(enc);
			unpad(msg);
			return msg;
		}
	};

	template <typename T>
	struct PublicKeyBase : public Key<T>
	{
		Bigint e{};

		PublicKeyBase() = default;

		PublicKeyBase(const Bigint& n, const Bigint& e)
			: Key<T>(n), e(e)
		{
		}

		[[nodiscard]] Bigint encryptPkcs1(std::string msg) const // non-deterministic
		{
			Key<T>::padPublic(msg);
			return Key<T>::encryptUnpadded(msg);
		}

		template <typename CryptoHashAlgo>
		[[nodiscard]] bool verify(const std::string& msg, const Bigint& sig) const
		{
			auto hash_bin = CryptoHashAlgo::hash(msg);
			return Key<T>::template padHash<CryptoHashAlgo>(hash_bin)
				&& Key<T>::decryptUnpadded(sig) == hash_bin;
		}
	};

	struct PublicKey : public PublicKeyBase<PublicKey>
	{
		static Bigint E_PREF;

		PublicKey() = default;
		PublicKey(const Bigint& n);
		PublicKey(const Bigint& n, const Bigint& e);

		[[nodiscard]] Bigint modPow(const Bigint& x) const;
	};

	struct KeyMontgomeryData
	{
		size_t re{};
		Bigint r{};
		Bigint n_mod_mul_inv{};
		Bigint r_mod_mul_inv{};
		Bigint one_mont{};

		KeyMontgomeryData() = default;
		KeyMontgomeryData(const Bigint& n, const Bigint& e);

		[[nodiscard]] Bigint modPow(const Bigint& n, const Bigint& e, const Bigint& x) const;
	};

	/*
	* In the case of an 1024-bit rsa public key, using a long-lived instance takes ~134ms, but performs operations in ~2ms, compared to
	* ~12ms using a short-lived instance. From these numbers, we can estimate that a long-lived instance is the right choice for rsa public
	* keys that are (expected to be) used more than 13 times.
	*/
	struct LonglivedPublicKey : public PublicKeyBase<LonglivedPublicKey>
	{
		KeyMontgomeryData mont_data;

		LonglivedPublicKey() = default;
		LonglivedPublicKey(const Bigint& n);
		LonglivedPublicKey(const Bigint& n, const Bigint& e);

		[[nodiscard]] Bigint modPow(const Bigint& x) const;
	};

	struct PrivateKey : public Key<PrivateKey>
	{
		Bigint p;
		Bigint q;
		Bigint dp;
		Bigint dq;
		Bigint qinv;

		KeyMontgomeryData p_mont_data;
		KeyMontgomeryData q_mont_data;

		PrivateKey() = default;
		PrivateKey(const Bigint& n, const Bigint& p, const Bigint& q, const Bigint& dp, const Bigint& dq, const Bigint& qinv);

		[[nodiscard]] static PrivateKey fromBinary(const std::string bin);
		[[nodiscard]] static PrivateKey fromAsn1(const Asn1Sequence& seq);
		[[nodiscard]] static PrivateKey fromJwk(const JsonObject& jwk);

		template <typename CryptoHashAlgo>
		[[nodiscard]] Bigint sign(const std::string& msg) const // deterministic
		{
			return encryptPkcs1(CryptoHashAlgo::hashWithId(msg));
		}

		[[nodiscard]] Bigint encryptPkcs1(std::string msg) const; // deterministic

		[[nodiscard]] PublicKey derivePublic() const; // assumes that e = e_pref, which is true unless your keypair is 21-bit or less.

		[[nodiscard]] Asn1Sequence toAsn1() const; // as per PKCS#1. assumes that e = e_pref, which is true unless your keypair is 21-bit or less.
		[[nodiscard]] std::string toPem() const; // assumes that e = e_pref, which is true unless your keypair is 21-bit or less.

		[[nodiscard]] Bigint modPow(const Bigint& x) const;
		[[nodiscard]] Bigint getE() const; // returns public exponent. assumes that e = e_pref, which is true unless your keypair is 21-bit or less.
		[[nodiscard]] Bigint getD() const; // returns private exponent. assumes that e = e_pref, which is true unless your keypair is 21-bit or less.
	};

	struct Keypair : public Mod
	{
		Bigint p;
		Bigint q;
		Bigint e;
		Bigint dp;
		Bigint dq;
		Bigint qinv;

		Keypair() = default;
		Keypair(Bigint&& _p, Bigint&& _q);

		[[nodiscard]] static Keypair random(unsigned int bits);

		[[nodiscard]] PublicKey getPublic() const;
		[[nodiscard]] PrivateKey getPrivate() const;
	};
}
