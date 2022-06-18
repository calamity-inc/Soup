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
			assert(soup::base64::encode("😀") == "8J+YgA==");
		});
		test("urlEncode", []
		{
			assert(soup::base64::urlEncode("Hello", true) == "SGVsbG8=");
			assert(soup::base64::urlEncode("😀", true) == "8J-YgA==");
		});
		test("decode", []
		{
			assert(soup::base64::decode("SGVsbG8=") == "Hello");
			assert(soup::base64::decode("8J+YgA==") == "😀");
		});
		test("urlDecode", []
		{
			assert(soup::base64::urlDecode("SGVsbG8=") == "Hello");
			assert(soup::base64::urlDecode("8J-YgA==") == "😀");
		});
	}
}
