#include "TreeReader.hpp"

#include "base.hpp"

#include "BitWriter.hpp"
#include "string.hpp"

NAMESPACE_SOUP
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
			auto value = getValue(child);
			if (!value.empty())
			{
				SOUP_ASSERT(value.find('\n') == std::string::npos);
				str.append(": ");
				str.append(value);
			}
			if (name.empty() && value.empty())
			{
				str.push_back(':');
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

	std::unordered_map<std::string, std::string> TreeReader::toMap(const void* root, bool disallow_empty_value) const
	{
		std::unordered_map<std::string, std::string> map;
		toMap(map, root, disallow_empty_value, {});
		return map;
	}

	void TreeReader::toMap(std::unordered_map<std::string, std::string>& map, const void* root, bool disallow_empty_value, const std::string& prefix) const
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
				toMap(map, child, disallow_empty_value, inner_prefix);
			}
			auto value = getValue(child);
			if (!disallow_empty_value || !value.empty())
			{
				map.emplace(std::move(name), std::move(value));
			}
		}
	}

	void TreeReader::toBinary(BitWriter& w, const void* root) const
	{
		SOUP_ASSERT(canHaveChildren(root));
		const size_t num_children = getNumChildren(root);
		for (size_t i = 0; i != num_children; ++i)
		{
			const void* const child = getChild(root, i);
			auto name = getName(child);
			SOUP_ASSERT(!name.empty());
			w.str_utf8_nt(name);
			w.str_utf8_nt(getValue(child));
			if (canHaveChildren(child))
			{
				toBinary(w, child);
			}
		}
		w.str_utf8_nt({});
	}
}
