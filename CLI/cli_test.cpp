#include "cli.hpp"

#include <unit_testing.hpp>

#include <base64.hpp>

#include <json.hpp>
#include <JsonArray.hpp>
#include <JsonBool.hpp>
#include <JsonInt.hpp>
#include <JsonObject.hpp>
#include <JsonString.hpp>

#include <PhpState.hpp>

using namespace soup;

void tests()
{
	unit("base64")
	{
		test("encode", []
		{
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
			assert(base64::decode("SGVsbG8=") == "Hello");
			assert(base64::decode("8J+YgA==") == "😀");
		});
		test("urlDecode", []
		{
			assert(base64::urlDecode("SGVsbG8=") == "Hello");
			assert(base64::urlDecode("8J-YgA==") == "😀");
		});
	}

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

	test("php", []
	{
		PhpState php;
		assert(php.evaluate(R"(<?php echo "Hello";)") == "Hello");
		assert(php.evaluate(R"(<?php echo 123;)") == "123");
		assert(php.evaluate(R"(<?php $a = 1; echo $a;)") == "1");
		assert(php.evaluate(R"(<?php $a = "Hello"; echo $a;)") == "Hello");
		assert(php.evaluate(R"(<?php $a = "Hello, "."world!"; echo $a;)") == "Hello, world!");
		assert(php.evaluate(R"(<?php $a = function(){ echo "Hello"; }; $a();)") == "Hello");
		assert(php.evaluate(R"(<?php $a = function(){ ?>Hello<?php }; $a();)") == "Hello");
		assert(php.evaluate(R"(<?php $a = function(){ ?>Hello<?php };)") == "");
		assert(php.evaluate(R"(<?php $a = function(){ $a = "Hello"; echo $a; }; $a();)") == "Hello");
		assert(php.evaluate(R"(<?php
//echo "Hello";
echo "Hello, world!";)") == "Hello, world!");
		assert(php.evaluate(R"(<?php echo 1 == 1;)") == "1"); // this is what vanilla PHP also prints, but "true" would be nicer
	});
}

void cli_test()
{
	unit("soup")
	{
		tests();
	}
}
