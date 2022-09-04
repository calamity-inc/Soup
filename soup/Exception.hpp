#pragma once

#include "base.hpp"

#include <stdexcept>
#include <string>

namespace soup
{
	struct Exception : public std::exception
	{
		using std::exception::exception;

		Exception(const std::string& str)
			: std::exception(str.c_str())
		{
		}

		[[noreturn]] static SOUP_FORCEINLINE void purecall()
		{
			throw Exception("Call to virtual function that was not implemented by specialisation");
		}
	};
}
