#pragma once

#include <stdexcept>

namespace soup
{
	struct Exception : public std::exception
	{
		using std::exception::exception;

		Exception(const std::string& str)
			: std::exception(str.c_str())
		{
		}
	};
}
