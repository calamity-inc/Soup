#pragma once

#include <string>

#include "fwd.hpp"

NAMESPACE_SOUP
{
	struct TreeWriter
	{
		virtual void* createChild(void* parent, std::string&& name, std::string&& value) const = 0;

		void fromBinary(Reader& r, void* root) const;
	};
}
