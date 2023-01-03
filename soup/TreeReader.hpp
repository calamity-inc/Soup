#pragma once

#include <string>

namespace soup
{
	struct TreeReader
	{
		[[nodiscard]] virtual std::string getName(const void* node) const = 0;
		[[nodiscard]] virtual std::string getValue(const void* node) const = 0;
		[[nodiscard]] virtual bool hasChildren(const void* node) const = 0;
		[[nodiscard]] virtual std::vector<const void*> getChildren(const void* node) const = 0;

		[[nodiscard]] std::string toString(const void* root, const std::string& prefix = {}) const
		{
			std::string str;
			for (const auto& child : getChildren(root))
			{
				str.append(prefix);
				str.append(getName(child));
				if (auto val = getValue(child); !val.empty())
				{
					str.append(": ");
					str.append(val);
				}
				str.push_back('\n');
				if (hasChildren(child))
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
