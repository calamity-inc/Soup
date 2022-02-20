#include "rsa.hpp"

#include <thread>

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
			str.insert(0, 1, rand.ch());
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
		for (auto i = str.rbegin(); *i != '\0'; ++i, --len)
		{
			if (i == str.rend())
			{
				return false;
			}
		}
		str.erase(0, len);
		return true;
	}

	// rsa::key_public

#if SOUP_BITS > 32
	bigint::chunk_t rsa::key_public::e_pref = 65537u;
#else
	bigint rsa::key_public::e_pref = 65537_b;
#endif

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

	bigint rsa::key_private::encryptPkcs1(std::string msg) const
	{
		padPrivate(msg);
		return encryptUnpadded(msg);
	}

	key_public rsa::key_private::derivePublic() const
	{
		return key_public(n);
	}

	bigint rsa::key_private::modPow(const bigint& x) const
	{
		auto mp = x.modPow(dp, p);
		auto mq = x.modPow(dq, q);
		auto h = (qinv * (mp - mq) % p);
		return ((mq + (h * q)) % n);
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
		bigint p, q;
		std::thread pt([&p, bits]
		{
			p = bigint::randomProbablePrime(bits);
		});
		std::thread qt([&q, bits]
		{
			q = bigint::randomProbablePrime(bits);
		});
		pt.join();
		qt.join();
		return keypair(std::move(p), std::move(q));
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
