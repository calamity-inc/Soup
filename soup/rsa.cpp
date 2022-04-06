#include "rsa.hpp"

#include "asn1_sequence.hpp"
#include "asn1_type.hpp"
#include "obfus_string.hpp"
#include "pem.hpp"
#include "promise.hpp"
#include "rand.hpp"

namespace soup
{
	using namespace literals;

	using key_public = rsa::key_public;
	using key_public_longlived = rsa::key_public_longlived;
	using key_private = rsa::key_private;
	using keypair = rsa::keypair;

	// rsa::mod

	size_t rsa::mod::getMaxUnpaddedMessageBytes() const
	{
		return n.getNumBytes();
	}

	size_t rsa::mod::getMaxPkcs1MessageBytes() const
	{
		return getMaxUnpaddedMessageBytes() - 11;
	}

	bool rsa::mod::padPublic(std::string& str) const
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

	bool rsa::mod::padPrivate(std::string& str) const
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

	bool rsa::mod::unpad(std::string& str)
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

	// rsa::key_montgomery_data

	rsa::key_montgomery_data::key_montgomery_data(const bigint& n, const bigint& e)
		: re(n.montgomeryREFromM()),
		r(bigint::montgomeryRFromRE(re)),
		one_mont(r.modUnsignedNotpowerof2(n))
	{
		bigint::modMulInv2Coprimes(n, r, n_mod_mul_inv, r_mod_mul_inv);
	}

	bigint rsa::key_montgomery_data::modPow(const bigint& n, const bigint& e, const bigint& x) const
	{
		return x.modPowMontgomery(e, re, r, n, r_mod_mul_inv, n_mod_mul_inv, one_mont);
	}

	// rsa::key_public

#define E_PREF 65537_b

	bigint rsa::key_public::e_pref = E_PREF;

	rsa::key_public::key_public(const bigint& n)
		: key_public(n, E_PREF)
	{
	}

	rsa::key_public::key_public(const bigint& n, const bigint& e)
		: key_public_base(n, e)
	{
	}

	bigint rsa::key_public::modPow(const bigint& x) const
	{
		return x.modPowBasic(e, n);
	}

	// rsa::key_public_longlived

	rsa::key_public_longlived::key_public_longlived(const bigint& n)
		: key_public_longlived(n, E_PREF)
	{
	}

	rsa::key_public_longlived::key_public_longlived(const bigint& n, const bigint& e)
		: key_public_base(n, e), mont_data(n, e)
	{
	}

	bigint rsa::key_public_longlived::modPow(const bigint& x) const
	{
		return mont_data.modPow(n, e, x);
	}

	// rsa::key_private

	rsa::key_private::key_private(const bigint& n, const bigint& p, const bigint& q, const bigint& dp, const bigint& dq, const bigint& qinv)
		: key(n), p(p), q(q), dp(dp), dq(dq), qinv(qinv),
		p_mont_data(p, dp),
		q_mont_data(q, dq)
	{
	}

	key_private rsa::key_private::fromBinary(const std::string bin)
	{
		return fromAsn1(asn1_sequence::fromBinary(bin));
	}

	key_private rsa::key_private::fromAsn1(const asn1_sequence& seq)
	{
		if (seq.getChildType(1).type != asn1_type::INTEGER)
		{
			// assuming that seq[1] is sequence containing OID 1.2.840.113549.1.1.1
			return fromAsn1(soup::asn1_sequence::fromBinary(seq.getString(2)));
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

	bigint rsa::key_private::encryptPkcs1(std::string msg) const
	{
		padPrivate(msg);
		return encryptUnpadded(msg);
	}

	key_public rsa::key_private::derivePublic() const
	{
		return key_public(n);
	}

	asn1_sequence rsa::key_private::toAsn1() const
	{
		asn1_sequence seq{};
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

	std::string rsa::key_private::toPem() const
	{
		return pem::encode(obfus_string("RSA PRIVATE KEY"), toAsn1().toDer());
	}

	bigint rsa::key_private::modPow(const bigint& x) const
	{
		auto mp = p_mont_data.modPow(p, dp, x);
		auto mq = q_mont_data.modPow(q, dq, x);
		auto h = (qinv * (mp - mq) % p);
		return ((mq + (h * q)) % n);
	}

	bigint rsa::key_private::getE() const
	{
		return key_public::e_pref;
	}

	bigint rsa::key_private::getD() const
	{
		return getE().modMulInv((p - 1_b).lcm(q - 1_b));
	}

	// rsa::keypair

	rsa::keypair::keypair(bigint&& _p, bigint&& _q)
		: mod(_p * _q), p(std::move(_p)), q(std::move(_q))
	{
		const auto pm1 = (p - 1_b);
		const auto qm1 = (q - 1_b);
		const auto t = pm1.lcm(qm1);
		if (t < key_public::e_pref)
		{
			const auto bl = t.getBitLength();
			do
			{
				e = bigint::randomProbablePrime(bl);
			} while (e >= t || e.isDivisorOf(t));
		}
		else
		{
			e = key_public::e_pref;
		}
		const auto d = e.modMulInv(t);
		dp = d.modUnsigned(pm1);
		dq = d.modUnsigned(qm1);
		qinv = q.modMulInv(p);
	}

	keypair rsa::keypair::random(unsigned int bits)
	{
		bits /= 2u;
		auto g = [](capture&& cap) -> bigint
		{
			return bigint::randomProbablePrime(cap.get<unsigned int>(), 3);
		};
		promise<bigint> p{ g, bits };
		promise<bigint> q{ g, bits };
		p.awaitCompletion();
		q.awaitCompletion();
		return keypair(std::move(p.getResult()), std::move(q.getResult()));
	}

	key_public rsa::keypair::getPublic() const
	{
		return key_public(n, e);
	}

	key_private rsa::keypair::getPrivate() const
	{
		return key_private(n, p, q, dp, dq, qinv);
	}
}
