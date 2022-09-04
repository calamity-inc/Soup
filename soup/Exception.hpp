#pragma once

#include <stdexcept>

namespace soup
{
	struct Exception : public std::exception
	{
		using std::exception::exception;
	};
}
