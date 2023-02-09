#pragma once

#include <string>

#include "fwd.hpp"

namespace soup
{
	struct TreeWriter
	{
		virtual void* createChild(void* parent, std::string&& name, std::string&& value) const = 0;

		void fromBinary(BitReader& r, void* root) const;
	};
}
