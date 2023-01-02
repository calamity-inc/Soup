#pragma once

namespace soup
{
	struct TreeReader
	{
		[[nodiscard]] virtual std::string getName(const void* node) const = 0;
		[[nodiscard]] virtual std::string getValue(const void* node) const = 0;
		[[nodiscard]] virtual bool hasChildren(const void* node) const = 0;
		[[nodiscard]] virtual std::vector<const void*> getChildren(const void* node) const = 0;
	};
}
