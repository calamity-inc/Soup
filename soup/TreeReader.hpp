#pragma once

#include <string>

namespace soup
{
	struct TreeReader
	{
		[[nodiscard]] virtual std::string getName(const void* node) const = 0;
		[[nodiscard]] virtual std::string getValue(const void* node) const = 0;
		[[nodiscard]] virtual bool canHaveChildren(const void* node) const { return true; }
		[[nodiscard]] virtual size_t getNumChildren(const void* node) const = 0;
		[[nodiscard]] virtual const void* getChild(const void* node, size_t i) const = 0;

		[[nodiscard]] std::string toString(const void* root, const std::string& prefix = {}) const;
	};
}
