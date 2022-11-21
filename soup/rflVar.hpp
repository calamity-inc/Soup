#pragma once

#include "rflType.hpp"

namespace soup
{
	struct rflVar
	{
		rflType type;
		std::string name;

		[[nodiscard]] std::string toString() const
		{
			std::string str = type.toString();
			str.push_back(' ');
			str.append(name);
			return str;
		}
	};
}
