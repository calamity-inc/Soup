#include "TreeReader.hpp"

#include "base.hpp"

namespace soup
{
	std::string TreeReader::toString(const void* root, const std::string& prefix) const
	{
		SOUP_ASSERT(canHaveChildren(root));
		std::string str;
		const size_t num_children = getNumChildren(root);
		for (size_t i = 0; i != num_children; ++i)
		{
			const void* const child = getChild(root, i);
			str.append(prefix);
			str.append(getName(child));
			if (auto val = getValue(child); !val.empty())
			{
				str.append(": ");
				str.append(val);
			}
			str.push_back('\n');
			if (canHaveChildren(child))
			{
				std::string inner_prefix = prefix;
				inner_prefix.push_back('\t');
				str.append(toString(child, inner_prefix));
			}
		}
		return str;
	}

	std::string TreeReader::toCat(const void* root, const std::string& prefix) const
	{
		SOUP_ASSERT(canHaveChildren(root));
		std::string str;
		const size_t num_children = getNumChildren(root);
		for (size_t i = 0; i != num_children; ++i)
		{
			const void* const child = getChild(root, i);
			str.append(prefix);
			str.append(getName(child));
			if (auto val = getValue(child); !val.empty())
			{
				str.append(": ");
				str.append(val);
			}
			str.push_back('\n');
			if (canHaveChildren(child))
			{
				std::string inner_prefix = prefix;
				inner_prefix.push_back('\t');
				str.append(toString(child, inner_prefix));
			}
		}
		return str;
	}
}
