#include "TreeReader.hpp"

#include "base.hpp"
#include "string.hpp"

namespace soup
{
	std::string TreeReader::toString(const void* root, const std::string& prefix) const
	{
		std::string str;
		SOUP_ASSERT(canHaveChildren(root));
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
		std::string str;
		SOUP_ASSERT(canHaveChildren(root));
		const size_t num_children = getNumChildren(root);
		for (size_t i = 0; i != num_children; ++i)
		{
			const void* const child = getChild(root, i);
			str.append(prefix);
			auto name = getName(child);
			if (name.find(':') != std::string::npos)
			{
				SOUP_ASSERT(name.find("\\:") == std::string::npos);
				string::replaceAll(name, ":", "\\:");
			}
			str.append(name);
			if (auto val = getValue(child); !val.empty())
			{
				SOUP_ASSERT(val.find('\n') == std::string::npos);
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

	std::unordered_map<std::string, std::string> TreeReader::toMap(const void* root) const
	{
		std::unordered_map<std::string, std::string> map;
		toMap(map, root);
		return map;
	}

	void TreeReader::toMap(std::unordered_map<std::string, std::string>& map, const void* root, const std::string& prefix) const
	{
		SOUP_ASSERT(canHaveChildren(root));
		const size_t num_children = getNumChildren(root);
		for (size_t i = 0; i != num_children; ++i)
		{
			const void* const child = getChild(root, i);
			auto name = getName(child);
			name.insert(0, prefix);
			if (canHaveChildren(child))
			{
				std::string inner_prefix = name;
				inner_prefix.push_back('>');
				toMap(map, child, inner_prefix);
			}
			map.emplace(std::move(name), getValue(child));
		}
	}
}
