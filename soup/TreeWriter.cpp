#include "TreeWriter.hpp"

#include "BitReader.hpp"

NAMESPACE_SOUP
{
	void TreeWriter::fromBinary(BitReader& r, void* root) const
	{
		while (true)
		{
			std::string name;
			if (!r.str_utf8_nt(name)
				|| name.empty()
				)
			{
				break;
			}
			std::string value;
			if (!r.str_utf8_nt(value))
			{
				break;
			}
			fromBinary(r, createChild(root, std::move(name), std::move(value)));
		}
	}
}
