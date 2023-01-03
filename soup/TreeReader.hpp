#pragma once

#include <string>

#include "base.hpp"

namespace soup
{
	struct TreeReader
	{
		[[nodiscard]] virtual std::string getName(const void* node) const = 0;
		[[nodiscard]] virtual std::string getValue(const void* node) const = 0;
		[[nodiscard]] virtual bool canHaveChildren(const void* node) const { return true; }
		[[nodiscard]] virtual size_t getNumChildren(const void* node) const = 0;
		[[nodiscard]] virtual const void* getChild(const void* node, size_t i) const = 0;

		[[nodiscard]] std::string toString(const void* root, const std::string& prefix = {}) const
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
	};
}
