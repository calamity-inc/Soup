#include "TreeWriter.hpp"

#include "BitReader.hpp"
#include "Reader.hpp"

NAMESPACE_SOUP
{
	void TreeWriter::fromBinary(Reader& r, void* root) const
	{
		while (true)
		{
			std::string name;
			if (!r.str_lp_u64_dyn_b(name)
				|| name.empty()
				)
			{
				break;
			}
			std::string value;
			if (!r.str_lp_u64_dyn_b(value))
			{
				break;
			}
			fromBinary(r, createChild(root, std::move(name), std::move(value)));
		}
	}
}
