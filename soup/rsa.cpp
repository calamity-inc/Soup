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

	// rsa::key_public

	bigint rsa::key_public::e_pref = 65537_b;

	bigint rsa::key_public::encryptPkcs1(std::string msg) const
	{
		padPublic(msg);
		return encryptUnpadded(msg);
	}

	bigint rsa::key_public::modPow(const bigint& x) const
	{
		return x.modPow(e, n);
	}

	// rsa::key_private

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
		auto mp = x.modPow(dp, p);
		auto mq = x.modPow(dq, q);
		auto h = (qinv * (mp - mq) % p);
		return ((mq + (h * q)) % n);
	}

	bigint rsa::key_private::getE() const
	{
		return key_public::e_pref;
	}

	bigint rsa::key_private::getD() const
	{
		return getE().modMulInv((p - bigint::ONE).lcm(q - bigint::ONE));
	}

	// rsa::keypair

	rsa::keypair::keypair(bigint&& _p, bigint&& _q)
		: mod(_p * _q), p(std::move(_p)), q(std::move(_q))
	{
		const auto pm1 = (p - bigint::ONE);
		const auto qm1 = (q - bigint::ONE);
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
			return bigint::randomProbablePrime(cap.get<unsigned int>());
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
