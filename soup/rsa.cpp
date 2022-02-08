#include "rsa.hpp"

#include <thread>

#include "pkcs1.hpp"

namespace soup
{
	using namespace literals;

	using key = rsa::key;
	using key_public = rsa::key_public;
	using key_private = rsa::key_private;

	size_t rsa::key::getMaxUnpaddedMessageBytes() const
	{
		return n.getNumBytes();
	}

	size_t rsa::key::getMaxPkcs1MessageBytes() const
	{
		return getMaxUnpaddedMessageBytes() - 11;
	}

	bigint rsa::key::modPow(const bigint& x) const
	{
		return x.modPow(e, n);
	}

	bigint rsa::key::encryptUnpadded(const std::string& msg) const
	{
		return modPow(bigint::fromMessage(msg));
	}

	std::string rsa::key::decryptPkcs1(const bigint& enc) const
	{
		auto msg = decryptUnpadded(enc);
		pkcs1::unpad(msg);
		return msg;
	}

	std::string rsa::key::decryptUnpadded(const bigint& enc) const
	{
		return modPow(enc).toMessage();
	}

	bigint rsa::key::sign(const bigint& hash) const
	{
		return modPow(hash);
	}

	bool rsa::key::verify(const bigint& hash, const bigint& sig) const
	{
		return modPow(sig) == hash;
	}

#if SOUP_PLATFORM_BITS > 32
	bigint::chunk_t rsa::key_public::e_pref = 65537u;
#else
	bigint rsa::key_public::e_pref = 65537_b;
#endif

	bigint rsa::key_public::encryptPkcs1(std::string msg) const
	{
		pkcs1::public_pad(msg, getMaxUnpaddedMessageBytes());
		return encryptUnpadded(msg);
	}

	key_public rsa::key_private::derivePublic() const
	{
		return key_public(n, key_public::e_pref);
	}

	bigint rsa::key_private::encryptPkcs1(std::string msg) const
	{
		pkcs1::private_pad(msg, getMaxUnpaddedMessageBytes());
		return encryptUnpadded(msg);
	}

	void rsa::keypair::random(int bits)
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
		fromPrimes(p, q);
	}

	void rsa::keypair::fromPrimes(const bigint& p, const bigint& q)
	{
		using namespace soup::literals;

		n = (p * q);
		const auto t = (p - 1_b).lcm(q - 1_b); // = n.reducedTotient()
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
		d = e.modMulInv(t);
	}

	key_public rsa::keypair::getPublic() const
	{
		return key_public(n, e);
	}

	key_private rsa::keypair::getPrivate() const
	{
		return key_private(n, d);
	}
}
