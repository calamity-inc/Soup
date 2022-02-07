#include "rsa.hpp"

#include <thread>

namespace soup
{
	using key = rsa::key;

	bigint rsa::key::modPow(const bigint& x) const
	{
		return x.modPow(e, n);
	}

	bigint rsa::key::encryptUnpadded(const std::string& msg) const
	{
		return modPow(bigint::fromMessage(msg));
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
		if (t < (bigint::chunk_t)65537u)
		{
			e = 65537_b;
		}
		else
		{
			const auto bl = t.getBitLength();
			do
			{
				e = bigint::randomProbablePrime(bl);
			} while (e >= t || e.isDivisorOf(t));
		}
		d = e.modMulInv(t);
	}

	key rsa::keypair::getPublic() const
	{
		return key{ n, e };
	}

	key rsa::keypair::getPrivate() const
	{
		return key{ n, d };
	}
}
