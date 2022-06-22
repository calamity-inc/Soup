#pragma once

#include <stdexcept>

namespace soup
{
	struct ParseError : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};
}
