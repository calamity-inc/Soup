#pragma once

#include "fwd.hpp"

#include "Bigint.hpp"
#include "JsonObject.hpp"

namespace soup
{
	struct RsaMod
	{
		Bigint n{};

		RsaMod() = default;

		RsaMod(const Bigint& n)
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

		[[nodiscard]] UniquePtr<JsonObject> publicToJwk(const Bigint& e) const;
		[[nodiscard]] std::string publicGetJwkThumbprint(const Bigint& e) const;
	};

	template <typename T>
	struct RsaKey : public RsaMod
	{
		using RsaMod::RsaMod;

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
	struct RsaPublicKeyBase : public RsaKey<T>
	{
		Bigint e{};

		RsaPublicKeyBase() = default;

		RsaPublicKeyBase(const Bigint& n, const Bigint& e)
			: RsaKey<T>(n), e(e)
		{
		}

		[[nodiscard]] Bigint encryptPkcs1(std::string msg) const // non-deterministic
		{
			RsaKey<T>::padPublic(msg);
			return RsaKey<T>::encryptUnpadded(msg);
		}

		template <typename CryptoHashAlgo>
		[[nodiscard]] bool verify(const std::string& msg, const Bigint& sig) const
		{
			auto hash_bin = CryptoHashAlgo::hash(msg);
			return RsaKey<T>::template padHash<CryptoHashAlgo>(hash_bin)
				&& RsaKey<T>::decryptUnpadded(sig) == hash_bin;
		}

		[[nodiscard]] UniquePtr<JsonObject> toJwk() const
		{
			return RsaKey<T>::publicToJwk(e);
		}

		[[nodiscard]] std::string getJwkThumbprint() const
		{
			return RsaKey<T>::publicGetJwkThumbprint(e);
		}
	};

	struct RsaPublicKey : public RsaPublicKeyBase<RsaPublicKey>
	{
		static Bigint E_PREF;

		RsaPublicKey() = default;
		RsaPublicKey(const Bigint& n);
		RsaPublicKey(const Bigint& n, const Bigint& e);

		[[nodiscard]] Bigint modPow(const Bigint& x) const;
	};

	struct RsaKeyMontgomeryData
	{
		size_t re{};
		Bigint r{};
		Bigint n_mod_mul_inv{};
		Bigint r_mod_mul_inv{};
		Bigint one_mont{};

		RsaKeyMontgomeryData() = default;
		RsaKeyMontgomeryData(const Bigint& n, const Bigint& e);

		[[nodiscard]] Bigint modPow(const Bigint& n, const Bigint& e, const Bigint& x) const;
	};

	/*
	* In the case of an 1024-bit rsa public key, using a long-lived instance takes ~134ms, but performs operations in ~2ms, compared to
	* ~12ms using a short-lived instance. From these numbers, we can estimate that a long-lived instance is the right choice for rsa public
	* keys that are (expected to be) used more than 13 times.
	*/
	struct RsaPublicKeyLonglived : public RsaPublicKeyBase<RsaPublicKeyLonglived>
	{
		RsaKeyMontgomeryData mont_data;

		RsaPublicKeyLonglived() = default;
		RsaPublicKeyLonglived(const Bigint& n);
		RsaPublicKeyLonglived(const Bigint& n, const Bigint& e);

		[[nodiscard]] Bigint modPow(const Bigint& x) const;
	};

	struct RsaPrivateKey : public RsaKey<RsaPrivateKey>
	{
		Bigint p;
		Bigint q;
		Bigint dp;
		Bigint dq;
		Bigint qinv;

		RsaKeyMontgomeryData p_mont_data;
		RsaKeyMontgomeryData q_mont_data;

		RsaPrivateKey() = default;
		RsaPrivateKey(const Bigint& n, const Bigint& p, const Bigint& q, const Bigint& dp, const Bigint& dq, const Bigint& qinv);

		[[nodiscard]] static RsaPrivateKey fromBinary(const std::string& bin);
		[[nodiscard]] static RsaPrivateKey fromAsn1(const Asn1Sequence& seq);
		[[nodiscard]] static RsaPrivateKey fromJwk(const JsonObject& jwk);

		template <typename CryptoHashAlgo>
		[[nodiscard]] Bigint sign(const std::string& msg) const // deterministic
		{
			return encryptPkcs1(CryptoHashAlgo::hashWithId(msg));
		}

		[[nodiscard]] Bigint encryptPkcs1(std::string msg) const; // deterministic

		[[nodiscard]] RsaPublicKey derivePublic() const; // assumes that e = e_pref, which is true unless your keypair is 21-bit or less.

		[[nodiscard]] Asn1Sequence toAsn1() const; // as per PKCS #1. assumes that e = e_pref, which is true unless your keypair is 21-bit or less.
		[[nodiscard]] std::string toPem() const; // assumes that e = e_pref, which is true unless your keypair is 21-bit or less.

		[[nodiscard]] Bigint modPow(const Bigint& x) const;
		[[nodiscard]] Bigint getE() const; // returns public exponent. assumes that e = e_pref, which is true unless your keypair is 21-bit or less.
		[[nodiscard]] Bigint getD() const; // returns private exponent. assumes that e = e_pref, which is true unless your keypair is 21-bit or less.

		[[nodiscard]] Asn1Sequence createCsr(const std::vector<std::string>& common_names) const; // returns a Certificate Signing Request as per PKCS #10.
	};

	struct RsaKeypair : public RsaMod
	{
		Bigint p;
		Bigint q;
		Bigint e;
		Bigint dp;
		Bigint dq;
		Bigint qinv;

		RsaKeypair() = default;
		RsaKeypair(Bigint _p, Bigint _q);

		[[nodiscard]] static RsaKeypair generate(unsigned int bits, bool lax_length_requirement = false);
		[[nodiscard]] static RsaKeypair generate(algRng& rng, algRng& aux_rng, unsigned int bits, bool lax_length_requirement = false);

		[[nodiscard]] RsaPublicKey getPublic() const;
		[[nodiscard]] RsaPrivateKey getPrivate() const;
	};
}
