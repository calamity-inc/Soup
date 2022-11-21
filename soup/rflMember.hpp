#pragma once

#include "rflVar.hpp"

namespace soup
{
	struct rflMember : public rflVar
	{
		enum Accessibility : uint8_t
		{
			PUBLIC,
			PROTECTED,
			PRIVATE,
		};

		Accessibility accessibility;
	};
}
