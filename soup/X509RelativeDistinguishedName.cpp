#include "X509RelativeDistinguishedName.hpp"

#include "Asn1Sequence.hpp"

namespace soup
{
	void X509RelativeDistinguishedName::read(const Asn1Sequence& seq)
	{
		for (size_t i = 0; i != seq.countChildren(); ++i)
		{
			auto kv = seq.getSeq(i).getSeq(0);
			emplace_back(kv.getOid(0), kv.getString(1));
		}
	}
}
