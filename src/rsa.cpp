#include "rsa.hpp"

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

    key rsa::keypair::getPublic() const
    {
        return key{ n, e };
    }

    key rsa::keypair::getPrivate() const
    {
		return key{ n, d };
    }
}
