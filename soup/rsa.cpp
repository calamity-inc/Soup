#include "rsa.hpp"

#include "Asn1Sequence.hpp"
#include "Asn1Type.hpp"
#include "base64.hpp"
#include "JsonString.hpp"
#include "ObfusString.hpp"
#include "pem.hpp"
#include "Promise.hpp"
#include "rand.hpp"

namespace soup
{
	using namespace literals;

	// Mod

	size_t RsaMod::getMaxUnpaddedMessageBytes() const
	{
		return n.getNumBytes();
	}

	size_t RsaMod::getMaxPkcs1MessageBytes() const
	{
		return getMaxUnpaddedMessageBytes() - 11;
	}

	bool RsaMod::padPublic(std::string& str) const
	{
		const auto len = str.length();
		const auto max_unpadded_len = getMaxUnpaddedMessageBytes();
		if (len + 11 > max_unpadded_len)
		{
			return false;
		}
		str.reserve(max_unpadded_len);
		str.insert(0, 1, '\0');
		for (size_t i = max_unpadded_len - len - 3; i != 0; --i)
		{
			str.insert(0, 1, rand.ch(1u));
		}
		str.insert(0, 1, '\2');
		//str.insert(0, 1, '\0');
		return true;
	}

	bool RsaMod::padPrivate(std::string& str) const
	{
		const auto len = str.length();
		const auto max_unpadded_len = getMaxUnpaddedMessageBytes();
		if (len + 11 > max_unpadded_len)
		{
			return false;
		}
		str.reserve(max_unpadded_len);
		str.insert(0, 1, '\0');
		str.insert(0, max_unpadded_len - len - 3, '\xff');
		str.insert(0, 1, '\1');
		//str.insert(0, 1, '\0');
		return true;
	}

	bool RsaMod::unpad(std::string& str)
	{
		size_t len = str.length();
		if (len > 11)
		{
			auto c = str.data();
			//if (*c++ == 0)
			{
				if (auto type = *c++; type == 1 || type == 2)
				{
					while (*c++ != '\0');
					str.erase(0, c - str.data());
					return true;
				}
			}
		}
		return false;
	}

	UniquePtr<JsonObject> RsaMod::publicToJwk(const Bigint& e) const
	{
		auto obj = soup::make_unique<JsonObject>();
		obj->add("kty", "RSA");
		obj->add("n", base64::urlEncode(n.toBinary()));
		obj->add("e", base64::urlEncode(e.toBinary()));
		return obj;
	}

	// KeyMontgomeryData

	RsaKeyMontgomeryData::RsaKeyMontgomeryData(const Bigint& n, const Bigint& e)
		: re(n.montgomeryREFromM()),
		r(Bigint::montgomeryRFromRE(re)),
		one_mont(r.modUnsignedNotpowerof2(n))
	{
		Bigint::modMulInv2Coprimes(n, r, n_mod_mul_inv, r_mod_mul_inv);
	}

	Bigint RsaKeyMontgomeryData::modPow(const Bigint& n, const Bigint& e, const Bigint& x) const
	{
		return x.modPowMontgomery(e, re, r, n, r_mod_mul_inv, n_mod_mul_inv, one_mont);
	}

	// PublicKey

#define E_PREF_VAL 65537_b

	Bigint RsaPublicKey::E_PREF = E_PREF_VAL;

	RsaPublicKey::RsaPublicKey(const Bigint& n)
		: RsaPublicKey(n, E_PREF_VAL)
	{
	}

	RsaPublicKey::RsaPublicKey(const Bigint& n, const Bigint& e)
		: RsaPublicKeyBase(n, e)
	{
	}

	Bigint RsaPublicKey::modPow(const Bigint& x) const
	{
		return x.modPowBasic(e, n);
	}

	// LonglivedPublicKey

	RsaPublicKeyLonglived::RsaPublicKeyLonglived(const Bigint& n)
		: RsaPublicKeyLonglived(n, E_PREF_VAL)
	{
	}

	RsaPublicKeyLonglived::RsaPublicKeyLonglived(const Bigint& n, const Bigint& e)
		: RsaPublicKeyBase(n, e), mont_data(n, e)
	{
	}

	Bigint RsaPublicKeyLonglived::modPow(const Bigint& x) const
	{
		return mont_data.modPow(n, e, x);
	}

	// PrivateKey

	RsaPrivateKey::RsaPrivateKey(const Bigint& n, const Bigint& p, const Bigint& q, const Bigint& dp, const Bigint& dq, const Bigint& qinv)
		: RsaKey(n), p(p), q(q), dp(dp), dq(dq), qinv(qinv),
		p_mont_data(p, dp),
		q_mont_data(q, dq)
	{
	}

	RsaPrivateKey RsaPrivateKey::fromBinary(const std::string& bin)
	{
		return fromAsn1(Asn1Sequence::fromBinary(bin));
	}

	RsaPrivateKey RsaPrivateKey::fromAsn1(const Asn1Sequence& seq)
	{
		if (seq.getChildType(1).type != Asn1Type::INTEGER)
		{
			// assuming that seq[1] is sequence containing OID 1.2.840.113549.1.1.1
			return fromAsn1(Asn1Sequence::fromBinary(seq.getString(2)));
		}
		return {
			seq.getInt(1),
			seq.getInt(4),
			seq.getInt(5),
			seq.getInt(6),
			seq.getInt(7),
			seq.getInt(8),
		};
	}

	RsaPrivateKey RsaPrivateKey::fromJwk(const JsonObject& jwk)
	{
		// assuming that jwk["kty"] == "RSA"
		return {
			Bigint::fromBinary(base64::urlDecode(*jwk.at("n")->asStr())),
			Bigint::fromBinary(base64::urlDecode(*jwk.at("p")->asStr())),
			Bigint::fromBinary(base64::urlDecode(*jwk.at("q")->asStr())),
			Bigint::fromBinary(base64::urlDecode(*jwk.at("dp")->asStr())),
			Bigint::fromBinary(base64::urlDecode(*jwk.at("dq")->asStr())),
			Bigint::fromBinary(base64::urlDecode(*jwk.at("qi")->asStr())),
		};
	}

	Bigint RsaPrivateKey::encryptPkcs1(std::string msg) const
	{
		padPrivate(msg);
		return encryptUnpadded(msg);
	}

	RsaPublicKey RsaPrivateKey::derivePublic() const
	{
		return RsaPublicKey(n);
	}

	Asn1Sequence RsaPrivateKey::toAsn1() const
	{
		Asn1Sequence seq{};
		/* 0 */ seq.addInt({}); // version (0)
		/* 1 */ seq.addInt(n);
		/* 2 */ seq.addInt(getE());
		/* 3 */ seq.addInt(getD());
		/* 4 */ seq.addInt(p);
		/* 5 */ seq.addInt(q);
		/* 6 */ seq.addInt(dp);
		/* 7 */ seq.addInt(dq);
		/* 8 */ seq.addInt(qinv);
		return seq;
	}

	std::string RsaPrivateKey::toPem() const
	{
		return pem::encode(ObfusString("RSA PRIVATE KEY"), toAsn1().toDer());
	}

	Bigint RsaPrivateKey::modPow(const Bigint& x) const
	{
		auto mp = p_mont_data.modPow(p, dp, x);
		auto mq = q_mont_data.modPow(q, dq, x);
		auto h = (qinv * (mp - mq) % p);
		return ((mq + (h * q)) % n);
	}

	Bigint RsaPrivateKey::getE() const
	{
		return RsaPublicKey::E_PREF;
	}

	Bigint RsaPrivateKey::getD() const
	{
		return getE().modMulInv((p - 1_b).lcm(q - 1_b));
	}

	// Keypair

	RsaKeypair::RsaKeypair(Bigint _p, Bigint _q)
		: RsaMod(_p * _q), p(std::move(_p)), q(std::move(_q))
	{
		const auto pm1 = (p - 1_b);
		const auto qm1 = (q - 1_b);
		const auto t = (pm1 * qm1);
		if (t < RsaPublicKey::E_PREF)
		{
			const auto bl = t.getBitLength();
			do
			{
				e = Bigint::randomProbablePrime(bl);
			} while (e >= t || e.isDivisorOf(t));
		}
		else
		{
			e = RsaPublicKey::E_PREF;
		}
		const auto d = e.modMulInv(t);
		dp = d.modUnsigned(pm1);
		dq = d.modUnsigned(qm1);
		qinv = q.modMulInv(p);
	}

	[[nodiscard]] static Bigint gen(unsigned int bits)
	{
		return Bigint::randomProbablePrime(bits, 3);
	}

	RsaKeypair RsaKeypair::generate(unsigned int bits)
	{
		auto gen_promise = [](Capture&& cap, PromiseBase*) -> Bigint
		{
			return gen(cap.get<unsigned int>());
		};

		std::vector<Bigint> primes{};
		{
			Promise<Bigint> p{ gen_promise, ((bits / 2u) - 2u) };
			Promise<Bigint> q{ gen_promise, ((bits / 2u) + 2u) };
			p.awaitCompletion();
			q.awaitCompletion();
			primes.emplace_back(p.getResult());
			primes.emplace_back(q.getResult());
		}

		while (true)
		{
			for (const auto& p : primes)
			{
				for (const auto& q : primes)
				{
					if (p != q)
					{
						RsaKeypair kp(p, q);
						if (kp.n.getBitLength() == bits)
						{
							return kp;
						}
					}
				}
			}

			primes.emplace_back(gen(bits / 2u));
		}
	}

	RsaPublicKey RsaKeypair::getPublic() const
	{
		return RsaPublicKey(n, e);
	}

	RsaPrivateKey RsaKeypair::getPrivate() const
	{
		return RsaPrivateKey(n, p, q, dp, dq, qinv);
	}
}
