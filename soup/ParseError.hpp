#pragma once

#include "Exception.hpp"

namespace soup
{
	struct ParseError : public Exception
	{
		using Exception::Exception;
	};
}
