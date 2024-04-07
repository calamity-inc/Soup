#pragma once

#include "TreeWriter.hpp"

#include "cat.hpp"

NAMESPACE_SOUP
{
	struct catTreeWriter : public TreeWriter
	{
		void* createChild(void* _parent, std::string&& name, std::string&& value) const final
		{
			auto* const parent = reinterpret_cast<catNode*>(_parent);
			return parent->children.emplace_back(soup::make_unique<catNode>(parent, std::move(name), std::move(value))).get();
		}
	};
}
