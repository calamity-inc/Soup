#pragma once

#include "TreeReader.hpp"

#include "cat.hpp"

NAMESPACE_SOUP
{
	struct catTreeReader : public TreeReader
	{
		[[nodiscard]] std::string getName(const void* node) const final
		{
			return reinterpret_cast<const catNode*>(node)->name;
		}

		[[nodiscard]] std::string getValue(const void* node) const final
		{
			return reinterpret_cast<const catNode*>(node)->value;
		}

		[[nodiscard]] size_t getNumChildren(const void* node) const final
		{
			return reinterpret_cast<const catNode*>(node)->children.size();
		}

		[[nodiscard]] const void* getChild(const void* node, size_t i) const final
		{
			return reinterpret_cast<const catNode*>(node)->children.at(i).get();
		}
	};
}
