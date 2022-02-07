#include "rsa.hpp"

#include <thread>

namespace soup
{
	using key = rsa::key;

	bigint rsa::key::pow_mod(const bigint& x) const
	{
		return x.pow_mod(e, n);
	}

	bigint rsa::key::encrypt(const bigint& msg) const
	{
		return pow_mod(msg);
	}

	bigint rsa::key::decrypt(const bigint& msg) const
	{
		return pow_mod(msg);
	}

	bigint rsa::key::sign(const bigint& hash) const
	{
		return pow_mod(hash);
	}

	bool rsa::key::verify(const bigint& hash, const bigint& sig) const
	{
		return pow_mod(sig) == hash;
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
			do
			{
				e = bigint::randomProbablePrime(t.getBitLength());
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
