#include "cli.hpp"

#include <unit_testing.hpp>

#include <base64.hpp>

void cli_test()
{
	unit("base64")
	{
		test("encode", []
		{
			assert(soup::base64::encode("Hello") == "SGVsbG8=");
		});
		test("decode", []
		{
			assert(soup::base64::decode("SGVsbG8=") == "Hello");
		});
	}
}
