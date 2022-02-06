#include "rsa.hpp"

namespace soup
{
    using key = rsa::key;

	bigint rsa::key::encrypt(const bigint& msg) const
	{
		bigint res = msg.pow(e);
		res %= n;
		return res;
	}

	bigint rsa::key::decrypt(const bigint& msg) const
	{
		return encrypt(msg);
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
