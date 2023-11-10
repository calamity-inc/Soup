#include "KeyGenId.hpp"

#include "rsa.hpp"
#include "StringRefReader.hpp"
#include "StringWriter.hpp"

namespace soup
{
	KeyGenId::KeyGenId(unsigned int bits, const std::string& seeds_export)
		: bits(bits)
	{
		StringRefReader sr(seeds_export);
		sr.u64_le(p.seed);
		sr.u64_le(q.seed);
	}

	KeyGenId KeyGenId::generate(unsigned int bits)
	{
		KeyGenId ret;
		ret.bits = bits;
		ret.p = BigintId::forRandomProbablePrime((bits / 2u) - 2u);
		ret.q = BigintId::forRandomProbablePrime((bits / 2u) + 2u);
		return ret;
	}

	std::string KeyGenId::toSeedsExport() const
	{
		StringWriter sw;
		sw.u64_le(const_cast<uint64_t&>(p.seed));
		sw.u64_le(const_cast<uint64_t&>(q.seed));
		return sw.data;
	}

	RsaKeypair KeyGenId::getKeypair() const
	{
		return RsaKeypair(p.getBigint((bits / 2u) - 2u), q.getBigint((bits / 2u) + 2u));
	}
}
