#pragma once

#include "base.hpp"

#include "Capture.hpp"
#include "Exception.hpp"

NAMESPACE_SOUP
{
	struct osException : public Exception
	{
		using Exception::Exception;
	};

	struct exceptions
	{
		// Handles low-level exceptions like access violation (segfault) and stack overflow and rethrows them as osException.
		static Capture isolate(Capture(*f)(Capture&&), Capture&& cap = {});

		[[nodiscard]] static std::string addr2name(const void* addr);

#if SOUP_WINDOWS
		[[nodiscard]] static std::string getName(_EXCEPTION_POINTERS* exp);
#endif
	};
}

/* Tiny test suite:

#include <iostream>

#include <exceptions.hpp>

using namespace soup;

static void overflow_the_stack(volatile unsigned int* param)
{
	volatile unsigned int dummy[256];
	dummy[*param] %= 256;
	overflow_the_stack(&dummy[*param]);
}

static void run_my_shit_and_dont_crash(void(*f)())
{
	try
	{
		exceptions::isolate([](Capture&& cap) -> Capture
		{
			cap.get<void(*)()>()();
			return {};
		}, f);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

int main()
{
	run_my_shit_and_dont_crash([]
	{
		*(volatile int*)0 = 0;
	});
	run_my_shit_and_dont_crash([]
	{
		unsigned int initial = 3; overflow_the_stack(&initial);
	});
	run_my_shit_and_dont_crash([]
	{
		unsigned int initial = 3; overflow_the_stack(&initial);
	});
	std::cout << "didn't crash :)\n";
}
*/
