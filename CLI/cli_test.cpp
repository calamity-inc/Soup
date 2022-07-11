#include "cli.hpp"

#include <unit_testing.hpp>

#include <search_match.hpp>

#include <SegWitAddress.hpp>
#include <Hotp.hpp>
#include <rsa.hpp>

#include <base32.hpp>
#include <base58.hpp>
#include <base64.hpp>
#include <ripemd160.hpp>
#include <sha1.hpp>
#include <sha256.hpp>

#include <json.hpp>
#include <JsonArray.hpp>
#include <JsonBool.hpp>
#include <JsonInt.hpp>
#include <JsonObject.hpp>
#include <JsonString.hpp>
#include <xml.hpp>

#include <BitReader.hpp>
#include <StringReader.hpp>

#include <PhpState.hpp>

#include <Uri.hpp>

#include <string.hpp>
#include <intutil.hpp>

using namespace soup;
using namespace soup::literals;

static void test_algos()
{
	test("search_match", []
	{
		assert(search_match("run program", "Run Script/Program") == true);
		assert(search_match("apple", "orange") == false);
		assert(search_match("bad", "The quick brown fox jumps over the lazy dog.") == false);
	});
}

static void test_crypto()
{
	test("SegWitAddress", []
	{
		SegWitAddress addr;
		addr.compressed_pub_key = Bigint::fromStringHex("0279be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798", 66).toBinary();
		assert(addr.encode() == "bc1qw508d6qejxtdg4y5r3zarvary0c5xw7kv8f3t4");
	});

	test("hotp", []
	{
		Hotp gen("12345678901234567890");
		assert(gen.getValue(0) == 755224);
		assert(gen.getValue(1) == 287082);
		assert(gen.getValue(2) == 359152);
		assert(gen.getValue(3) == 969429);
		assert(gen.getValue(4) == 338314);
		assert(gen.getValue(5) == 254676);
		assert(gen.getValue(6) == 287922);
		assert(gen.getValue(7) == 162583);
		assert(gen.getValue(8) == 399871);
		assert(gen.getValue(9) == 520489);
	});

	test("rsa", []
	{
		rsa::Keypair kp(
			"96529209707922958264660626622151327182265565708623147261613126577409795199887"_b,
			"87505296413890087200392682183900465764322220376584167643884573751015402662091"_b
		);
		auto enc = "3939991117139809241563517827579718715756222298160587806559781632547966505691296013680068230342942841852094486596819343548681582442588753971618922157744527"_b;
		assert(kp.getPrivate().encryptUnpadded("Soup") == enc);
		assert(kp.getPublic().decryptUnpadded(enc) == "Soup");
	});
}

static void test_data()
{
	unit("base32")
	{
		test("encode", []
		{
			assert(base32::encode("a") == "ME======");
			assert(base32::encode("aa") == "MFQQ====");
			assert(base32::encode("aaa") == "MFQWC===");
			assert(base32::encode("aaaa") == "MFQWCYI=");
			assert(base32::encode("aaaaa") == "MFQWCYLB");
			assert(base32::encode("aaaaaa") == "MFQWCYLBME======");
		});
		test("decode", []
		{
			assert(base32::decode("ME======") == "a");
			assert(base32::decode("MFQQ====") == "aa");
			assert(base32::decode("MFQWC===") == "aaa");
			assert(base32::decode("MFQWCYI=") == "aaaa");
			assert(base32::decode("MFQWCYLB") == "aaaaa");
			assert(base32::decode("MFQWCYLBME======") == "aaaaaa");
		});
	}

	unit("base58")
	{
		test("decode", []
		{
			assert(string::bin2hex(base58::decode("5HueCGU8rMjxEXxiPuD5BDku4MkFqeZyd4dZ1jvhTVqvbTLvyTJ")) == "800C28FCA386C7A227600B2FE50B7CAE11EC86D3BF1FBE471BE89827E19D72AA1D507A5B8D");
		});
	}
	
	unit("base64")
	{
		test("encode", []
		{
			assert(base64::encode("a") == "YQ==");
			assert(base64::encode("Hello") == "SGVsbG8=");
			assert(base64::encode("😀") == "8J+YgA==");
		});
		test("urlEncode", []
		{
			assert(base64::urlEncode("Hello", true) == "SGVsbG8=");
			assert(base64::urlEncode("😀", true) == "8J-YgA==");
		});
		test("decode", []
		{
			assert(base64::decode("YQ==") == "a");
			assert(base64::decode("SGVsbG8=") == "Hello");
			assert(base64::decode("8J+YgA==") == "😀");
		});
		test("urlDecode", []
		{
			assert(base64::urlDecode("SGVsbG8=") == "Hello");
			assert(base64::urlDecode("8J-YgA==") == "😀");
		});
	}

	test("ripemd160", []
	{
		assert(string::bin2hexLower(soup::ripemd160("The quick brown fox jumps over the lazy dog")) == "37f332f68db77bd9d7edd4969571ad671cf9dd3b");
		assert(string::bin2hexLower(soup::ripemd160("The quick brown fox jumps over the lazy cog")) == "132072df690933835eb8b6ad0b77e7b6f14acad7");
		assert(string::bin2hexLower(soup::ripemd160("")) == "9c1185a5c5e9fc54612808977ee8f548b2258d31");
	});

	test("sha1", []
	{
		assert(sha1::hash("") == std::string("\xda\x39\xa3\xee\x5e\x6b\x4b\x0d\x32\x55\xbf\xef\x95\x60\x18\x90\xaf\xd8\x07\x09", 20));
		assert(sha1::hash("abc") == std::string("\xa9\x99\x3e\x36\x47\x06\x81\x6a\xba\x3e\x25\x71\x78\x50\xc2\x6c\x9c\xd0\xd8\x9d", 20));
	});

	test("sha256", []
	{
		assert(sha256::hash("") == std::string("\xe3\xb0\xc4\x42\x98\xfc\x1c\x14\x9a\xfb\xf4\xc8\x99\x6f\xb9\x24\x27\xae\x41\xe4\x64\x9b\x93\x4c\xa4\x95\x99\x1b\x78\x52\xb8\x55", 32));
		assert(sha256::hash("abc") == std::string("\xba\x78\x16\xbf\x8f\x01\xcf\xea\x41\x41\x40\xde\x5d\xae\x22\x23\xb0\x03\x61\xa3\x96\x17\x7a\x9c\xb4\x10\xff\x61\xf2\x00\x15\xad", 32));
	});

	test("sha256 hmac", []
	{
		assert(sha256::hmac("Hi There", "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b") == std::string("\xb0\x34\x4c\x61\xd8\xdb\x38\x53\x5c\xa8\xaf\xce\xaf\x0b\xf1\x2b\x88\x1d\xc2\x00\xc9\x83\x3d\xa7\x26\xe9\x37\x6c\x2e\x32\xcf\xf7", 32));
	});

	test("sha256 tls prf", []
	{
		assert(sha256::tls_prf("test label", 100,
			"\x9b\xbe\x43\x6b\xa9\x40\xf0\x17\xb1\x76\x52\x84\x9a\x71\xdb\x35",
			"\xa0\xba\x9f\x93\x6c\xda\x31\x18\x27\xa6\xf7\x96\xff\xd5\x19\x8c")
			== std::string("\xe3\xf2\x29\xba\x72\x7b\xe1\x7b\x8d\x12\x26\x20\x55\x7c\xd4\x53\xc2\xaa\xb2\x1d\x07\xc3\xd4\x95\x32\x9b\x52\xd4\xe6\x1e\xdb\x5a\x6b\x30\x17\x91\xe9\x0d\x35\xc9\xc9\xa4\x6b\x4e\x14\xba\xf9\xaf\x0f\xa0\x22\xf7\x07\x7d\xef\x17\xab\xfd\x37\x97\xc0\x56\x4b\xab\x4f\xbc\x91\x66\x6e\x9d\xef\x9b\x97\xfc\xe3\x4f\x79\x67\x89\xba\xa4\x80\x82\xd1\x22\xee\x42\xc5\xa7\x2e\x5a\x51\x10\xff\xf7\x01\x87\x34\x7b\x66", 100)
		);
	});

	test("json", []
	{
		auto tree = json::decodeForDedicatedVariable(R"({
"firstName": "John",
"lastName": "Smith",
"isAlive": true,
"age": 27,
"address": {
"streetAddress": "21 2nd Street",
"city": "New York",
"state": "NY",
"postalCode": "10021-3100"
},
"phoneNumbers": [
{
	"type": "home",
	"number": "212 555-1234"
},
{
	"type": "office",
	"number": "646 555-4567"
}
],
"children": [],
"spouse": null
}
)");
		auto obj = tree->asObj();
		assert(*obj->at("firstName")->asStr() == "John");
		assert(*obj->at("lastName")->asStr() == "Smith");
		assert(*obj->at("isAlive")->asBool() == true);
		assert(*obj->at("age")->asInt() == 27);
		assert(*obj->at("phoneNumbers")->asArr()->at(0).asObj()->at("type")->asStr() == "home");
		assert(obj->at("spouse")->isNull());
	});

	test("xml", []
	{
		UniquePtr<XmlTag> tag;

		tag = xml::parse("<html></html>"); assert(tag->encode() == "<html></html>");
		tag = xml::parse("<html>Hello</html>"); assert(tag->encode() == "<html>Hello</html>");
		tag = xml::parse("<html>Hello"); assert(tag->encode() == "<html>Hello</html>");
		tag = xml::parse("<html>"); assert(tag->encode() == "<html></html>");
		tag = xml::parse("<html><body>Hello</body></html>"); assert(tag->encode() == "<html><body>Hello</body></html>");
		tag = xml::parse(R"(<html lang="en">Hello</html>)"); assert(tag->encode() == R"(<html lang="en">Hello</html>)");
		tag = xml::parse(R"(<html><body/>test)"); assert(tag->encode() == R"(<html><body></body>test</html>)");
		tag = xml::parse(R"(<html><body><h1></body>test)"); assert(tag->encode() == R"(<html><body><h1></h1></body>test</html>)");
	});
}

static void test_io()
{
	test("BitReader", []
	{
		StringReader r("\xF0\x0F");
		BitReader br(&r);
		uint8_t b;

		assert(br.readByte(4, b) && b == 0x0);
		assert(br.readByte(4, b) && b == 0xF);
		assert(br.readByte(4, b) && b == 0xF);
		assert(br.readByte(4, b) && b == 0x0);

		r = "\xE0\x03";
		assert(br.readByte(5, b) && b == 0);
		assert(br.readByte(5, b) && b == 0b11111);
		assert(br.readByte(6, b) && b == 0);
	});
}

static void test_lang()
{
	unit("php")
	{
		test("echo", []
		{
			PhpState php;
			assert(php.evaluate(R"(<?php echo "Hello";)") == "Hello");
			assert(php.evaluate(R"(<?php echo 123;)") == "123");
		});
		test("variables", []
		{
			PhpState php;
			assert(php.evaluate(R"(<?php $a = "Hello"; echo $a;)") == "Hello");
			assert(php.evaluate(R"(<?php $a = 1; echo $a;)") == "1");
			assert(php.evaluate(R"(<?php
$a = "Hi";
function f()
{
	$a = "Hello";
}
f();
echo $a;
)") == "Hi");
		});
		test("concat", []
		{
			PhpState php;
			assert(php.evaluate(R"(<?php $a = "Hello"; echo $a;)") == "Hello");
			assert(php.evaluate(R"(<?php $a = "Hello, "."world!"; echo $a;)") == "Hello, world!");
		});
		test("functions", []
		{
			PhpState php;
			assert(php.evaluate(R"(<?php $a = function(){ echo "Hello"; }; $a();)") == "Hello");
			assert(php.evaluate(R"(<?php $a = function(){ ?>Hello<?php }; $a();)") == "Hello");
			assert(php.evaluate(R"(<?php $a = function(){ ?>Hello<?php };)") == "");
			assert(php.evaluate(R"(<?php $a = function(){ $a = "Hello"; echo $a; }; $a();)") == "Hello");
			assert(php.evaluate(R"(<?php function a() { echo "Hello"; } a();)") == "Hello");
			assert(php.evaluate(R"(<?php function a($a) { echo $a; } a("Hi");)") == "Hi");
			assert(php.evaluate(R"(<?php
function greet($greeting, $subject)
{
	echo $greeting.", ".$subject."!";
}
greet("Hello", "world");)") == "Hello, world!");
		});
		test("comments", []
		{
			PhpState php;
			assert(php.evaluate(R"(<?php
//echo "Hello";
echo "Hello, world!";)") == "Hello, world!");
		});
		test("comparisons", []
		{
			PhpState php;
			assert(php.evaluate(R"(<?php echo 1 == 1;)") == "1");
		});
		test("conditionals", []
		{
			PhpState php;
			assert(php.evaluate(R"(<?php
$a = 1;
if($a == 1)
{
	echo "true";
}
else
{
	echo "false";
})") == "true");
			assert(php.evaluate(R"(<?php
$a = 2;
if($a == 1)
{
	echo "true";
}
else
{
	echo "false";
})") == "false");
			assert(php.evaluate(R"(<?php
$a = 1;
if($a == 1):
echo "Hello";
endif;)") == "Hello");
			assert(php.evaluate(R"(<?php
$a = 1;
if($a == 2):
echo "Hello";
endif;)") == "");
		});
		test("comparisons", []
		{
			PhpState php;
			php.request_uri = "/1337";
			assert(php.evaluate(R"(<?php echo "Request URI: ".$_SERVER["REQUEST_URI"];)") == "Request URI: /1337");
		});
	}
}

static void test_uri()
{
	std::string str;
	Uri uri;

	str = "//google.com";
	uri = Uri(str);
	assert(uri.scheme == "");
	assert(uri.host == "google.com");
	assert(uri.port == 0);
	assert(uri.user == "");
	assert(uri.pass == "");
	assert(uri.path == "");
	assert(uri.query == "");
	assert(uri.fragment == "");
	assert(uri.toString() == str);

	str = "//google.com/";
	uri = Uri(str);
	assert(uri.scheme == "");
	assert(uri.host == "google.com");
	assert(uri.port == 0);
	assert(uri.user == "");
	assert(uri.pass == "");
	assert(uri.path == "/");
	assert(uri.query == "");
	assert(uri.fragment == "");
	assert(uri.toString() == str);

	str = "//shady.nz/path#anchor";
	uri = Uri(str);
	assert(uri.scheme == "");
	assert(uri.host == "shady.nz");
	assert(uri.port == 0);
	assert(uri.user == "");
	assert(uri.pass == "");
	assert(uri.path == "/path");
	assert(uri.query == "");
	assert(uri.fragment == "anchor");
	assert(uri.toString() == str);

	str = "http://username:password@hostname:9090/path?arg=value#anchor";
	uri = Uri(str);
	assert(uri.scheme == "http");
	assert(uri.host == "hostname");
	assert(uri.port == 9090);
	assert(uri.user == "username");
	assert(uri.pass == "password");
	assert(uri.path == "/path");
	assert(uri.query == "arg=value");
	assert(uri.fragment == "anchor");
	assert(uri.toString() == str);
}

static void test_util()
{
	test("bin2hex", []
	{
		assert(string::bin2hex("\x1\x2\x3") == "010203");
	});
	test("invertEndianness", []
	{
		assert(intutil::invertEndianness(0x1234567890ABCDEFull) == 0xEFCDAB9078563412ull);
		assert(intutil::invertEndianness(0xEFCDAB9078563412ull) == 0x1234567890ABCDEFull);
	});
	test("pow", []
	{
		assert(intutil::pow(10, 6) == 1000000);
	});
}

void cli_test()
{
	unit("soup")
	{
		unit("algos")
		{
			test_algos();
		}
		unit("crypto")
		{
			test_crypto();
		}
		unit("data")
		{
			test_data();
		}
		unit("io")
		{
			test_io();
		}
		unit("lang")
		{
			test_lang();
		}
		unit("net")
		{
			unit("web")
			{
				test("uri", test_uri);
			}
		}
		unit("util")
		{
			test_util();
		}
	}
}
