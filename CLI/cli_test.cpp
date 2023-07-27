#include "cli.hpp"

#include <unit_testing.hpp>

#include <x64.hpp>

#include <SegWitAddress.hpp>
#include <Hotp.hpp>
#include <rsa.hpp>

#include <base32.hpp>
#include <base58.hpp>
#include <base64.hpp>
#include <cat.hpp>
#include <ripemd160.hpp>
#include <sha1.hpp>
#include <sha256.hpp>

#include <json.hpp>
#include <JsonArray.hpp>
#include <JsonBool.hpp>
#include <JsonInt.hpp>
#include <JsonObject.hpp>
#include <JsonString.hpp>
#include <Regex.hpp>
#include <xml.hpp>

#include <BitReader.hpp>
#include <StringReader.hpp>
#include <BitWriter.hpp>
#include <StringWriter.hpp>

#include <PhpState.hpp>

#include <rflParser.hpp>
#include <rflStruct.hpp>

#include <cbCmd.hpp>
#include <cbParser.hpp>
#include <Chatbot.hpp>

#include <math.hpp>

#include <Uri.hpp>

#include <Socket.hpp>

#include <StringMatch.hpp>
#include <format.hpp>

#include <string.hpp>
#include <version_compare.hpp>

#include <Rgb.hpp>

using namespace soup;
using namespace soup::literals;

static void unit_cpu()
{
	test("x64", []
	{
#define ASSERT_X64_DISASM(asm, ...) { uint8_t code[] = { __VA_ARGS__ }; const uint8_t* cp = code; assert(x64Disasm(cp).toString() == asm); assert(cp == &code[sizeof(code)]); }

		ASSERT_X64_DISASM("mov al, al", 0x88, 0xC0);
		ASSERT_X64_DISASM("mov r8, r8", 0x4D, 0x89, 0xC0);
		ASSERT_X64_DISASM("mov ah, ah", 0x88, 0xE4);
		ASSERT_X64_DISASM("mov ax, ax", 0x66, 0x89, 0xC0);
		ASSERT_X64_DISASM("mov eax, eax", 0x89, 0xC0);
		ASSERT_X64_DISASM("mov rax, rax", 0x48, 0x89, 0xC0);

		ASSERT_X64_DISASM("mov [eax], eax", 0x67, 0x89, 0x00);
		ASSERT_X64_DISASM("mov [eax], rax", 0x67, 0x48, 0x89, 0x00);
		ASSERT_X64_DISASM("mov [rax], eax", 0x89, 0x00);
		ASSERT_X64_DISASM("mov [rax], rax", 0x48, 0x89, 0x00);

		ASSERT_X64_DISASM("mov al, [eax]", 0x67, 0x8A, 0x00);
		ASSERT_X64_DISASM("mov ah, [eax]", 0x67, 0x8A, 0x20);
		ASSERT_X64_DISASM("mov ax, [eax]", 0x67, 0x66, 0x8B, 0x00);
		ASSERT_X64_DISASM("mov eax, [eax]", 0x67, 0x8B, 0x00);
		ASSERT_X64_DISASM("mov eax, [rax]", 0x8B, 0x00);
		ASSERT_X64_DISASM("mov rax, [eax]", 0x67, 0x48, 0x8B, 0x00);
		ASSERT_X64_DISASM("mov rax, [rax]", 0x48, 0x8B, 0x00);

		ASSERT_X64_DISASM("mov [rsp], rax", 0x48, 0x89, 0x04, 0x24);
		ASSERT_X64_DISASM("mov [rsp+0x69], rbx", 0x48, 0x89, 0x5C, 0x24, 0x69);
		ASSERT_X64_DISASM("mov [rax+0x69], rbx", 0x48, 0x89, 0x58, 0x69);

		ASSERT_X64_DISASM("mov al, 69", 0xB0, 0x45);
		ASSERT_X64_DISASM("mov ah, 69", 0xB4, 0x45);
		ASSERT_X64_DISASM("mov ax, 69", 0x66, 0xB8, 0x45, 0x00);
		ASSERT_X64_DISASM("mov eax, 69", 0xB8, 0x45, 0x00, 0x00, 0x00);
		ASSERT_X64_DISASM("mov rax, 69", 0x48, 0xC7, 0xC0, 0x45, 0x00, 0x00, 0x00);
		ASSERT_X64_DISASM("mov rax, 1384798605843239735", 0x48, 0xB8, 0x37, 0x13, 0xBE, 0xBA, 0xFE, 0xCA, 0x37, 0x13);

		ASSERT_X64_DISASM("mov rbp, rbp", 0x48, 0x89, 0xED);
		ASSERT_X64_DISASM("mov byte ptr [rip+0x69], 0", 0xC6, 0x05, 0x69, 0x00, 0x00, 0x00, 0x00);
		ASSERT_X64_DISASM("mov byte ptr [rbp+0x69], 0", 0xC6, 0x45, 0x69, 0x00);

		ASSERT_X64_DISASM("mov 0, 69", 0xC7, 0x04, 0x25, 0x00, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x00);

		ASSERT_X64_DISASM("ret", 0xC3);

		ASSERT_X64_DISASM("push ax", 0x66, 0x50);
		ASSERT_X64_DISASM("push rax", 0x50);
		ASSERT_X64_DISASM("push rdi", 0x57);
		ASSERT_X64_DISASM("push r15", 0x41, 0x57);

		ASSERT_X64_DISASM("push word ptr [rax]", 0x66, 0xFF, 0x30);
		ASSERT_X64_DISASM("push word ptr [r15]", 0x66, 0x41, 0xFF, 0x37);
		ASSERT_X64_DISASM("push qword ptr [rax]", 0xFF, 0x30);

		ASSERT_X64_DISASM("sub rsp, 32", 0x48, 0x83, 0xEC, 0x20);

		ASSERT_X64_DISASM("cmp al, al", 0x38, 0xc0);
		ASSERT_X64_DISASM("cmp rax, rax", 0x48, 0x39, 0xc0);
		ASSERT_X64_DISASM("cmp al, 0", 0x3c, 0x00);
		ASSERT_X64_DISASM("cmp rax, 0", 0x48, 0x83, 0xf8, 0x00);
		ASSERT_X64_DISASM("cmp rax, 1762867049", 0x48, 0x3d, 0x69, 0x37, 0x13, 0x69);
		ASSERT_X64_DISASM("cmp byte ptr [rax+0x69], 0", 0x80, 0x78, 0x69, 0x00);
		ASSERT_X64_DISASM("cmp byte ptr [rax+0x69133769], 0", 0x80, 0xb8, 0x69, 0x37, 0x13, 0x69, 0x00);
		ASSERT_X64_DISASM("cmp byte ptr [rip+0x69], 0", 0x80, 0x3d, 0x69, 0x00, 0x00, 0x00, 0x00);

#define ASSERT_X64_ASM(asm, ...) { uint8_t code[] = { __VA_ARGS__ }; x64Instruction ins; ins.fromString(asm); assert(memcmp(ins.toBytecode().c_str(), code, sizeof(code)) == 0); }

		ASSERT_X64_ASM("mov rax, rax", 0x48, 0x89, 0xC0); // The 0x8B opcode would also be valid for two direct-access registers.
	});
}

static void unit_crypto()
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
		RsaKeypair kp(
			"96529209707922958264660626622151327182265565708623147261613126577409795199887"_b,
			"87505296413890087200392682183900465764322220376584167643884573751015402662091"_b
		);
		auto enc = "3939991117139809241563517827579718715756222298160587806559781632547966505691296013680068230342942841852094486596819343548681582442588753971618922157744527"_b;
		assert(kp.getPrivate().encryptUnpadded("Soup") == enc);
		assert(kp.getPublic().decryptUnpadded(enc) == "Soup");
	});
}

static void unit_data()
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
			assert(base32::encode("0.1.2.3.4.5.6.7.8.9.10.11.12.13.14.15.16.17.18.19.20.21.22.23.24.25.26.27.28.29.30.31.32.33.34.35.36.37.38.39.40.41.42.43.44.45.46.47.48.49.50.51.52.53.54.55.56.57.58.59.60.61.62.63.64.65.66.67.68.69.70.71.72.73.74.75.76.77.78.79.80.81.82.83.84.85.86.87.88.89.90.91.92.93.94.95.96.97.98.99.100.101.102.103.104.105.106.107.108.109.110.111.112.113.114.115.116.117.118.119.120.121.122.123.124.125.126.127.128.129.130.131.132.133.134.135.136.137.138.139.140.141.142.143.144.145.146.147.148.149.150.151.152.153.154.155.156.157.158.159.160.161.162.163.164.165.166.167.168.169.170.171.172.173.174.175.176.177.178.179.180.181.182.183.184.185.186.187.188.189.190.191.192.193.194.195.196.197.198.199.200.201.202.203.204.205.206.207.208.209.210.211.212.213.214.215.216.217.218.219.220.221.222.223.224.225.226.227.228.229.230.231.232.233.234.235.236.237.238.239.240.241.242.243.244.245.246.247.248.249.250.251.252.253.254.255") == "GAXDCLRSFYZS4NBOGUXDMLRXFY4C4OJOGEYC4MJRFYYTELRRGMXDCNBOGE2S4MJWFYYTOLRRHAXDCOJOGIYC4MRRFYZDELRSGMXDENBOGI2S4MRWFYZDOLRSHAXDEOJOGMYC4MZRFYZTELRTGMXDGNBOGM2S4MZWFYZTOLRTHAXDGOJOGQYC4NBRFY2DELRUGMXDINBOGQ2S4NBWFY2DOLRUHAXDIOJOGUYC4NJRFY2TELRVGMXDKNBOGU2S4NJWFY2TOLRVHAXDKOJOGYYC4NRRFY3DELRWGMXDMNBOGY2S4NRWFY3DOLRWHAXDMOJOG4YC4NZRFY3TELRXGMXDONBOG42S4NZWFY3TOLRXHAXDOOJOHAYC4OBRFY4DELRYGMXDQNBOHA2S4OBWFY4DOLRYHAXDQOJOHEYC4OJRFY4TELRZGMXDSNBOHE2S4OJWFY4TOLRZHAXDSOJOGEYDALRRGAYS4MJQGIXDCMBTFYYTANBOGEYDKLRRGA3C4MJQG4XDCMBYFYYTAOJOGEYTALRRGEYS4MJRGIXDCMJTFYYTCNBOGEYTKLRRGE3C4MJRG4XDCMJYFYYTCOJOGEZDALRRGIYS4MJSGIXDCMRTFYYTENBOGEZDKLRRGI3C4MJSG4XDCMRYFYYTEOJOGEZTALRRGMYS4MJTGIXDCMZTFYYTGNBOGEZTKLRRGM3C4MJTG4XDCMZYFYYTGOJOGE2DALRRGQYS4MJUGIXDCNBTFYYTINBOGE2DKLRRGQ3C4MJUG4XDCNBYFYYTIOJOGE2TALRRGUYS4MJVGIXDCNJTFYYTKNBOGE2TKLRRGU3C4MJVG4XDCNJYFYYTKOJOGE3DALRRGYYS4MJWGIXDCNRTFYYTMNBOGE3DKLRRGY3C4MJWG4XDCNRYFYYTMOJOGE3TALRRG4YS4MJXGIXDCNZTFYYTONBOGE3TKLRRG43C4MJXG4XDCNZYFYYTOOJOGE4DALRRHAYS4MJYGIXDCOBTFYYTQNBOGE4DKLRRHA3C4MJYG4XDCOBYFYYTQOJOGE4TALRRHEYS4MJZGIXDCOJTFYYTSNBOGE4TKLRRHE3C4MJZG4XDCOJYFYYTSOJOGIYDALRSGAYS4MRQGIXDEMBTFYZDANBOGIYDKLRSGA3C4MRQG4XDEMBYFYZDAOJOGIYTALRSGEYS4MRRGIXDEMJTFYZDCNBOGIYTKLRSGE3C4MRRG4XDEMJYFYZDCOJOGIZDALRSGIYS4MRSGIXDEMRTFYZDENBOGIZDKLRSGI3C4MRSG4XDEMRYFYZDEOJOGIZTALRSGMYS4MRTGIXDEMZTFYZDGNBOGIZTKLRSGM3C4MRTG4XDEMZYFYZDGOJOGI2DALRSGQYS4MRUGIXDENBTFYZDINBOGI2DKLRSGQ3C4MRUG4XDENBYFYZDIOJOGI2TALRSGUYS4MRVGIXDENJTFYZDKNBOGI2TK===");
		});
		test("decode", []
		{
			assert(base32::decode("ME======") == "a");
			assert(base32::decode("MFQQ====") == "aa");
			assert(base32::decode("MFQWC===") == "aaa");
			assert(base32::decode("MFQWCYI=") == "aaaa");
			assert(base32::decode("MFQWCYLB") == "aaaaa");
			assert(base32::decode("MFQWCYLBME======") == "aaaaaa");
			assert(base32::decode("GAXDCLRSFYZS4NBOGUXDMLRXFY4C4OJOGEYC4MJRFYYTELRRGMXDCNBOGE2S4MJWFYYTOLRRHAXDCOJOGIYC4MRRFYZDELRSGMXDENBOGI2S4MRWFYZDOLRSHAXDEOJOGMYC4MZRFYZTELRTGMXDGNBOGM2S4MZWFYZTOLRTHAXDGOJOGQYC4NBRFY2DELRUGMXDINBOGQ2S4NBWFY2DOLRUHAXDIOJOGUYC4NJRFY2TELRVGMXDKNBOGU2S4NJWFY2TOLRVHAXDKOJOGYYC4NRRFY3DELRWGMXDMNBOGY2S4NRWFY3DOLRWHAXDMOJOG4YC4NZRFY3TELRXGMXDONBOG42S4NZWFY3TOLRXHAXDOOJOHAYC4OBRFY4DELRYGMXDQNBOHA2S4OBWFY4DOLRYHAXDQOJOHEYC4OJRFY4TELRZGMXDSNBOHE2S4OJWFY4TOLRZHAXDSOJOGEYDALRRGAYS4MJQGIXDCMBTFYYTANBOGEYDKLRRGA3C4MJQG4XDCMBYFYYTAOJOGEYTALRRGEYS4MJRGIXDCMJTFYYTCNBOGEYTKLRRGE3C4MJRG4XDCMJYFYYTCOJOGEZDALRRGIYS4MJSGIXDCMRTFYYTENBOGEZDKLRRGI3C4MJSG4XDCMRYFYYTEOJOGEZTALRRGMYS4MJTGIXDCMZTFYYTGNBOGEZTKLRRGM3C4MJTG4XDCMZYFYYTGOJOGE2DALRRGQYS4MJUGIXDCNBTFYYTINBOGE2DKLRRGQ3C4MJUG4XDCNBYFYYTIOJOGE2TALRRGUYS4MJVGIXDCNJTFYYTKNBOGE2TKLRRGU3C4MJVG4XDCNJYFYYTKOJOGE3DALRRGYYS4MJWGIXDCNRTFYYTMNBOGE3DKLRRGY3C4MJWG4XDCNRYFYYTMOJOGE3TALRRG4YS4MJXGIXDCNZTFYYTONBOGE3TKLRRG43C4MJXG4XDCNZYFYYTOOJOGE4DALRRHAYS4MJYGIXDCOBTFYYTQNBOGE4DKLRRHA3C4MJYG4XDCOBYFYYTQOJOGE4TALRRHEYS4MJZGIXDCOJTFYYTSNBOGE4TKLRRHE3C4MJZG4XDCOJYFYYTSOJOGIYDALRSGAYS4MRQGIXDEMBTFYZDANBOGIYDKLRSGA3C4MRQG4XDEMBYFYZDAOJOGIYTALRSGEYS4MRRGIXDEMJTFYZDCNBOGIYTKLRSGE3C4MRRG4XDEMJYFYZDCOJOGIZDALRSGIYS4MRSGIXDEMRTFYZDENBOGIZDKLRSGI3C4MRSG4XDEMRYFYZDEOJOGIZTALRSGMYS4MRTGIXDEMZTFYZDGNBOGIZTKLRSGM3C4MRTG4XDEMZYFYZDGOJOGI2DALRSGQYS4MRUGIXDENBTFYZDINBOGI2DKLRSGQ3C4MRUG4XDENBYFYZDIOJOGI2TALRSGUYS4MRVGIXDENJTFYZDKNBOGI2TK===") == "0.1.2.3.4.5.6.7.8.9.10.11.12.13.14.15.16.17.18.19.20.21.22.23.24.25.26.27.28.29.30.31.32.33.34.35.36.37.38.39.40.41.42.43.44.45.46.47.48.49.50.51.52.53.54.55.56.57.58.59.60.61.62.63.64.65.66.67.68.69.70.71.72.73.74.75.76.77.78.79.80.81.82.83.84.85.86.87.88.89.90.91.92.93.94.95.96.97.98.99.100.101.102.103.104.105.106.107.108.109.110.111.112.113.114.115.116.117.118.119.120.121.122.123.124.125.126.127.128.129.130.131.132.133.134.135.136.137.138.139.140.141.142.143.144.145.146.147.148.149.150.151.152.153.154.155.156.157.158.159.160.161.162.163.164.165.166.167.168.169.170.171.172.173.174.175.176.177.178.179.180.181.182.183.184.185.186.187.188.189.190.191.192.193.194.195.196.197.198.199.200.201.202.203.204.205.206.207.208.209.210.211.212.213.214.215.216.217.218.219.220.221.222.223.224.225.226.227.228.229.230.231.232.233.234.235.236.237.238.239.240.241.242.243.244.245.246.247.248.249.250.251.252.253.254.255");
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

	test("cat", []
	{
		StringReader sr("Hello: World");
		auto tree = catParse(sr);
		assert(tree->children.size() == 1);
		assert(tree->children.at(0)->name == "Hello");
		assert(tree->children.at(0)->value == "World");

		sr = (
			"Hello: World\n"
			"Name-only List\n"
			"\tName-only value\n"
			"List: With Value\n"
			"\tChild: With Value\n"
			"\tNesting\n"
			"\t\tNesting\n"
			"Final\n"
			"\n"
		);
		tree = catParse(sr);
		assert(tree->children.size() == 4);
		assert(tree->children.at(0)->name == "Hello");
		assert(tree->children.at(0)->value == "World");
		assert(tree->children.at(1)->name == "Name-only List");
		assert(tree->children.at(1)->value.empty());
		assert(tree->children.at(2)->name == "List");
		assert(tree->children.at(2)->value == "With Value");
		assert(tree->children.at(3)->name == "Final");
		assert(tree->children.at(3)->value.empty());

		sr = ("Colon (\\:): Valid");
		tree = catParse(sr);
		assert(tree->children.size() == 1);
		assert(tree->children.at(0)->name == "Colon (:)");
		assert(tree->children.at(0)->value == "Valid");

		sr = (
			"List: With Value\n"
			"  Child: With Value\n"
			"  Nesting\n"
			"    Nesting\n"
		);
		tree = catParse(sr);
		assert(tree->children.size() == 1);
		assert(tree->children.at(0)->name == "List");
		assert(tree->children.at(0)->value == "With Value");
		assert(tree->children.at(0)->children.size() == 2);
		assert(tree->children.at(0)->children.at(0)->name == "Child");
		assert(tree->children.at(0)->children.at(0)->value == "With Value");
		assert(tree->children.at(0)->children.at(1)->name == "Nesting");
		assert(tree->children.at(0)->children.at(1)->value.empty());

	});

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
		assert(sha1::hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == std::string("\x8c\xd9\x6a\xf2\x17\xb5\x19\x86\x55\xe7\x37\x80\xf3\x5d\x52\x2e\xba\x76\x22\x44", 20));
	});

	test("sha256", []
	{
		assert(sha256::hash("") == std::string("\xe3\xb0\xc4\x42\x98\xfc\x1c\x14\x9a\xfb\xf4\xc8\x99\x6f\xb9\x24\x27\xae\x41\xe4\x64\x9b\x93\x4c\xa4\x95\x99\x1b\x78\x52\xb8\x55", 32));
		assert(sha256::hash("abc") == std::string("\xba\x78\x16\xbf\x8f\x01\xcf\xea\x41\x41\x40\xde\x5d\xae\x22\x23\xb0\x03\x61\xa3\x96\x17\x7a\x9c\xb4\x10\xff\x61\xf2\x00\x15\xad", 32));
		assert(sha256::hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == std::string("\x13\x28\x44\x68\x1d\x57\xd5\xae\x76\xdf\xd3\x5a\x07\xe4\xfc\x10\xfd\xc0\xce\x9f\x08\x82\xa2\xb0\xc9\xdb\x0e\x84\x11\x6d\x5b\xad", 32));
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
		{
			auto tree = json::decode(R"({
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
	"number": "212 555-1234",
	"emptyobject": {}
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
			assert(tree);
			JsonObject& obj = tree->asObj();
			assert(obj.at("firstName").asStr() == "John");
			assert(obj.at("lastName").asStr() == "Smith");
			assert(obj.at("isAlive").asBool() == true);
			assert(obj.at("age").asInt() == 27);
			assert(obj.at("phoneNumbers").asArr().at(0).asObj().at("type").asStr() == "home");
			assert(obj.at("spouse").isNull());
		}
		{
			auto tree = json::decode(R"(// This is a comment

/* This is a comment
spanning over multiple lines */

{
"body":"Hello, world!"
}
			)");
			assert(tree);
			JsonObject& obj = tree->asObj();
			assert(obj.at("body").asStr() == "Hello, world!");
		}
		{
			auto tree = json::decode(R"(
{
// This is the body
"body":"Hello, world!"
}
			)");
			assert(tree);
			JsonObject& obj = tree->asObj();
			assert(obj.at("body").asStr() == "Hello, world!");
		}
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
		tag = xml::parse(R"(<img src="soup"/>)"); assert(tag->encode() == R"(<img src="soup"></img>)");

		// Handle multiple tags by implicitly creating a <body> tag
		tag = xml::parse("<p>foo</p><p>bar</p>"); assert(tag->encode() == "<body><p>foo</p><p>bar</p></body>");

		tag = xml::parse("<p>&amp;&lt;&gt;</p>"); assert(tag->encode() == "<p>&amp;&lt;&gt;</p>");
	});

	test("Endianness", []
	{
		assert(Endianness::invert((uint16_t)0x1234u) == 0x3412u);
		assert(Endianness::invert((uint16_t)0x3412u) == 0x1234u);
		assert(Endianness::invert((uint32_t)0x12345678u) == 0x78563412u);
		assert(Endianness::invert((uint32_t)0x78563412u) == 0x12345678u);
		assert(Endianness::invert((uint64_t)0x1234567890ABCDEFull) == 0xEFCDAB9078563412ull);
		assert(Endianness::invert((uint64_t)0xEFCDAB9078563412ull) == 0x1234567890ABCDEFull);
	});

	test("Regex", []
	{
		assert(Regex("ABC|XYZ").matches("ABC") == true);
		assert(Regex("ABC|XYZ").matches("AB") == false);
		assert(Regex("ABC|XYZ").matches("XYZ") == true);
		assert(Regex("ABC|XYZ").matches("XY") == false);

		assert(Regex("(A|B)C").matches("AC") == true);
		assert(Regex("(A|B)C").matches("AD") == false);
		assert(Regex("(A|B)C").matches("BC") == true);
		assert(Regex("(A|B)C").matches("BD") == false);

		assert(Regex("A(.B|.C)D").match("ABCD").toString() == R"(0="ABCD", 1="BC")");
		assert(Regex("A(?:.B|.C)D").match("ABCD").toString() == R"(0="ABCD")");
		assert(Regex("(?'deez'abc)").match("abc").toString() == R"(0="abc", 1{deez}="abc")");

		assert(Regex(".").matches("\n") == false);
		assert(Regex(".", "s").matches("\n") == true);

		assert(Regex("^ABC").matches("ABC") == true);
		assert(Regex(".^ABC", "sm").matches("\nABC") == true);
		assert(Regex(".^ABC", "s").matches("\nABC") == false);

		assert(Regex("ABC$").matches("ABC") == true);
		assert(Regex("ABC$").matches("ABC\n") == true);
		assert(Regex("ABC$", "D").matches("ABC\n") == false);

		assert(Regex("[abc]+").matchesFully("abc") == true);
		assert(Regex("[abc]+").matchesFully("abcdef") == false);
		assert(Regex("[a-z]+").matchesFully("abc") == true);
		assert(Regex("[a-z]+").matchesFully("abcdef") == true);
		assert(Regex("[^def]+").match("abcdef").toString() == R"(0="abc")");

		assert(Regex("A?BC").matches("BC") == true);
		assert(Regex("A?BC").matches("ABC") == true);
		assert(Regex("A?BC").matches("DBC") == false);

		assert(Regex("").match("ABC").toString() == R"(0="")");

		assert(Regex("AB*C").matchesFully("AC") == true);
		assert(Regex("AB*C").matchesFully("ABC") == true);
		assert(Regex("AB*C").matchesFully("ABBC") == true);

		assert(Regex("A(?=BC)B").match("AB").toString() == "");
		assert(Regex("A(?=BC)B").match("ABC").toString() == R"(0="AB")");
		assert(Regex("A(?=B+C)B").match("ABC").toString() == R"(0="AB")");
		assert(Regex("A(?=)B").matchesFully("AB"));

		assert(Regex("A(?!BC)B").match("AB").toString() == R"(0="AB")");
		assert(Regex("A(?!BC)B").match("ABC").toString() == "");
		assert(Regex("A(?!)B").match("AB").toString() == "");

		assert(Regex("A(?!B)").matches("A") == true);
		assert(Regex("A(?!B)").matches("AB") == false);

		assert(Regex("A{0}B").matchesFully("B") == true);
		assert(Regex("A{0}B").matches("AB") == false);
		assert(Regex("A{3}B").matches("AAB") == false);
		assert(Regex("A{3}B").matchesFully("AAAB") == true);

		assert(Regex("(?<=A)BC").search("BC").toString() == "");
		assert(Regex("(?<=A)BC").search("ABC").toString() == R"(0="BC")");
		assert(Regex("(?<=A)BC").search("DBC").toString() == "");

		assert(Regex("(?<!A)BC").search("BC").toString() == R"(0="BC")");
		assert(Regex("(?<!A)BC").search("ABC").toString() == "");
		assert(Regex("(?<!A)BC").search("DBC").toString() == R"(0="BC")");

		assert(Regex(R"(\babc\b)").search("abc").toString() == R"(0="abc")");
		assert(Regex(R"(\babc\b)").search(" abc ").toString() == R"(0="abc")");
		assert(Regex(R"(\babc\b)").search("_abc_").toString() == "");

		assert(Regex(R"(\Bdef)").search("abcdef").toString() == R"(0="def")");
		assert(Regex(R"(\Bdef)").search("abc def").toString() == "");
		assert(Regex(R"(\Bdef)").search("def").toString() == "");

		assert(Regex("(abc|)").matchesFully("abc") == true);
		assert(Regex("(abc|)").matchesFully("") == true);
		assert(Regex("(abc|)").matchesFully("abcdef") == false);

		assert(Regex(".*B").match("ABAB").toString() == R"(0="ABAB")");
		assert(Regex(".*?B").match("ABAB").toString() == R"(0="AB")");
		assert(Regex(".+B").match("ABAB").toString() == R"(0="ABAB")");
		assert(Regex(".+?B").match("ABAB").toString() == R"(0="AB")");

		assert(Regex(".*B", RE_UNGREEDY).match("ABAB").toString() == R"(0="AB")");
		assert(Regex(".*?B", RE_UNGREEDY).match("ABAB").toString() == R"(0="ABAB")");

		assert(Regex(".").matchesFully("💯") == false);
		assert(Regex(".", RE_UNICODE).matchesFully("💯") == true);

		assert(Regex("(?<=^.)A").search("💯A").isSuccess() == false);
		assert(Regex("(?<=^.)A", RE_UNICODE).search("💯A").toString() == R"(0="A")");
		
		assert(Regex("\\w\\W\\w").matchesFully("A B") == true);

		assert(Regex("\\AA\\Z").matches("A") == true);
		assert(Regex("\\AA\\Z").matches("A\n") == true);
		assert(Regex("\\AA\\z").matches("A") == true);
		assert(Regex("\\AA\\z").matches("A\n") == false);

		SOUP_UNUSED(Regex("A{3,0}")); // this shouldn't infinitely loop at parse-time
		SOUP_UNUSED(Regex("A{3,1}")); // this shouldn't infinitely loop at parse-time
		SOUP_UNUSED(Regex("A{3,2}")); // this shouldn't infinitely loop at parse-time
		SOUP_UNUSED(Regex("A{3,3}")); // this shouldn't infinitely loop at parse-time

		assert(Regex("A{3,6}").matchesFully("A") == false);
		assert(Regex("A{3,6}").matchesFully("AA") == false);
		assert(Regex("A{3,6}").matchesFully("AAA") == true);
		assert(Regex("A{3,6}").matchesFully("AAAA") == true);
		assert(Regex("A{3,6}").matchesFully("AAAAA") == true);
		assert(Regex("A{3,6}").matchesFully("AAAAAA") == true);
		assert(Regex("A{3,6}").matchesFully("AAAAAAA") == false);

		assert(Regex("A{3,}B").matchesFully("B") == false);
		assert(Regex("A{3,}B").matchesFully("AB") == false);
		assert(Regex("A{3,}B").matchesFully("AAB") == false);
		assert(Regex("A{3,}B").matchesFully("AAAB") == true);
		assert(Regex("A{3,}B").matchesFully("AAAAB") == true);
	});
}

static void unit_io()
{
	test("BitReader", []
	{
		StringReader r("\xF0\x0F");
		BitReader br(&r);
		uint8_t b;

		assert(br.u8(4, b) && b == 0x0);
		assert(br.u8(4, b) && b == 0xF);
		assert(br.u8(4, b) && b == 0xF);
		assert(br.u8(4, b) && b == 0x0);

		r = "\xE0\x03";
		assert(br.u8(5, b) && b == 0);
		assert(br.u8(5, b) && b == 0b11111);
		assert(br.u8(6, b) && b == 0);
	});
	test("BitWriter", []
	{
		StringWriter w;
		BitWriter bw(&w);

		w.data.clear();
		bw.u8(4, 0x0);
		bw.u8(4, 0xF);
		bw.u8(4, 0xF);
		bw.u8(4, 0x0);
		bw.finishByte();
		assert(w.data == "\xF0\x0F");

		w.data.clear();
		bw.u8(5, 0b11111);
		bw.u8(5, 0);
		bw.u8(6, 0b111111);
		bw.finishByte();
		assert(w.data == "\x1F\xFC");

		w.data.clear();
		bw.u8(5, 0b11111);
		bw.u8(4, 0b101);
		bw.finishByte();
		assert(w.data == "\x5F\x01");
	});
}

static void unit_lang()
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

	test("reflection", []
	{
		rflParser par(R"EOC(struct Person { const char* name; int age; })EOC");
		auto t = par.readStruct();
		assert(t.name == "Person");
		assert(t.members.size() == 2);
		assert(t.members.at(0).type.name == "const char");
		assert(t.members.at(0).type.toString() == "const char*");
		assert(t.members.at(0).name == "name");
		assert(t.members.at(1).type.name == "int");
		assert(t.members.at(1).type.toString() == "int");
		assert(t.members.at(1).name == "age");
	});
}

static bool test_chatbot_trigger(const std::string& text, const std::string& expected_trigger)
{
	cbParser p(text);
	for (const auto& cmd : Chatbot::getAllCommands())
	{
		for (const auto& trigger : cmd->getTriggers())
		{
			if (p.checkTrigger(trigger))
			{
				std::string trigger_str = *p.command_begin;
				if (p.command_begin != p.command_end)
				{
					auto i = p.command_begin + 1;
					while (true)
					{
						trigger_str.push_back(' ');
						trigger_str.append(*i);
						if (i == p.command_end)
						{
							break;
						}
						++i;
					}
				}
				return trigger_str == expected_trigger;
			}
		}
	}
	return false;
}

static void test_chatbot_triggers()
{
#define ASSERT_TRIGGER(text, expected_trigger) assert(test_chatbot_trigger(text, expected_trigger));

	ASSERT_TRIGGER("hi", "hi");
	ASSERT_TRIGGER("Hi!", "Hi!");
	ASSERT_TRIGGER("define autonomous", "define");
	ASSERT_TRIGGER("Define autonomous", "Define");
	ASSERT_TRIGGER("Can you define autonomous?", "define");
	ASSERT_TRIGGER("definition of autonomous", "definition of");
	ASSERT_TRIGGER("flip a coin", "flip a coin");
	ASSERT_TRIGGER("123 + 456", "+");
	ASSERT_TRIGGER("123 - 456", "-");
	ASSERT_TRIGGER("123 * 456", "*");
	ASSERT_TRIGGER("123 / 456", "/");
}

static void test_chatbot_args()
{
#define ASSERT_ARG(text, trigger, getArg, arg) { cbParser p(text); p.checkTrigger(trigger); assert(p.hasCommand()); assert(p.getArg() == arg); }

	ASSERT_ARG("define autonomous", "define", getArgWord, "autonomous");
	ASSERT_ARG("Can you define autonomous?", "define", getArgWord, "autonomous");
	ASSERT_ARG("definition of autonomous", "definition of", getArgWord, "autonomous");
	ASSERT_ARG("123 + 456", "+", getArgNumericLefthand, "123");
	ASSERT_ARG("123 + 456", "+", getArgNumeric, "456");
	ASSERT_ARG("Number between 1 and 10.", "number between", getArgNumeric, "1");
	ASSERT_ARG("Number between 1 and 10.", "number between", getArgNumericSecond, "10");
}

static void test_chatbot_implementables()
{
	{
		auto res = Chatbot::process("delete 10");
		assert(res.type == CB_RES_DELETE);
		assert(res.getDeleteArgs().num == 10);
	}
}

static void unit_math()
{
	test("pow", []
	{
		assert(soup::pow(10, 6) == 1000000);
	});
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

	str = "https://google.com";
	uri = Uri(str);
	assert(uri.scheme == "https");
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

	str = "https://web-safety.net/..;@www.google.com:%3443/";
	uri = Uri(str);
	assert(uri.scheme == "https");
	assert(uri.host == "web-safety.net");
	assert(uri.port == 0);
	assert(uri.user == "");
	assert(uri.pass == "");
	assert(uri.path == "/..;@www.google.com:443/");
	assert(uri.query == "");
	assert(uri.fragment == "");
	// Not testing uri.toString because this one is very malformatted so it's expected that Soup re-encodes it more sanely.

	str = "https://google.com?q=Soup";
	uri = Uri(str);
	assert(uri.scheme == "https");
	assert(uri.host == "google.com");
	assert(uri.port == 0);
	assert(uri.user == "");
	assert(uri.pass == "");
	assert(uri.path == "");
	assert(uri.query == "q=Soup");
	assert(uri.fragment == "");
	assert(uri.toString() == str);

	str = "https://google.com#q=Soup";
	uri = Uri(str);
	assert(uri.scheme == "https");
	assert(uri.host == "google.com");
	assert(uri.port == 0);
	assert(uri.user == "");
	assert(uri.pass == "");
	assert(uri.path == "");
	assert(uri.query == "");
	assert(uri.fragment == "q=Soup");
	assert(uri.toString() == str);

	str = "//example.com:443";
	uri = Uri(str);
	assert(uri.scheme == "");
	assert(uri.host == "example.com");
	assert(uri.port == 443);
	assert(uri.user == "");
	assert(uri.pass == "");
	assert(uri.path == "");
	assert(uri.query == "");
	assert(uri.fragment == "");
	assert(uri.toString() == str);
}

static void test_socket_raii_semantics()
{
	Socket s;
	assert(!s.hasConnection());
	s.fd = 1337;
	assert(s.hasConnection());
	Socket s2 = std::move(s);
	assert(!s.hasConnection());
	assert(s2.hasConnection());
	Socket s3;
	s3 = std::move(s2);
	assert(!s2.hasConnection());
	assert(s3.hasConnection());
}

static void unit_util_string()
{
	test("equalsIgnoreCase", []
	{
		assert(string::equalsIgnoreCase<std::string>("java", "java") == true);
		assert(string::equalsIgnoreCase<std::string>("Java", "java") == true);
		assert(string::equalsIgnoreCase<std::string>("Java", "Lava") == false);
		assert(string::equalsIgnoreCase<std::string>("Java", "Javaa") == false);
	});
	test("levenshtein", []
	{
		assert(string::levenshtein<std::string>("successfully", "success") == 5);
		assert(string::levenshtein<std::string>("alpha", "scope") == 5);
		assert(string::levenshtein<std::string>("equal", "equal") == 0);
		assert(string::levenshtein<std::string>("Hello, world!", "The sun is shining and it lets me know the day has just begun.") == 57);
	});
	test("StringMatch::search", []
	{
		assert(StringMatch::search("run program", "Run Script/Program") == true);
		assert(StringMatch::search("apple", "orange") == false);
		assert(StringMatch::search("bad", "The quick brown fox jumps over the lazy dog.") == false);
		assert(StringMatch::search("the bow", "The quick brown fox jumps over the lazy dog.") == false);
		assert(StringMatch::search("the same", "the same") == true);
		assert(StringMatch::search("play level", "Play next level") == true);
		assert(StringMatch::search("play next", "Play next level") == true);
		assert(StringMatch::search("find", "Find Objective") == true);
		assert(StringMatch::search("find", "Finish Dagger") == false);
	});
	test("StringMatch::wildcard", []
	{
		assert(StringMatch::wildcard("*.cpp", "foo.cpp") == true);
		assert(StringMatch::wildcard("*.cpp", "foo.bar.cpp") == true);

		assert(StringMatch::wildcard("*.*.cpp", "f.cpp") == false);
		assert(StringMatch::wildcard("*.*.cpp", "fo.cpp") == false);
		assert(StringMatch::wildcard("*.*.cpp", "foo.cpp") == false);
		assert(StringMatch::wildcard("*.*.cpp", "fooo.cpp") == false);
		assert(StringMatch::wildcard("*.*.cpp", "foooo.cpp") == false);
		assert(StringMatch::wildcard("*.*.cpp", "foo.bar.cpp") == true);

		assert(StringMatch::wildcard("****.cpp", "foo.cpp", 0) == true);
		assert(StringMatch::wildcard("***.cpp", "foo.cpp", 1) == true);
		assert(StringMatch::wildcard("****.cpp", "foo.cpp", 1) == false);

		assert(StringMatch::wildcard("lib*.so", "libsoup.so") == true);

		assert(StringMatch::wildcard("lib*_ext*.cpp", "libsoup_extspoon.cpp") == true);
	});
	test("format", []
	{
		assert(format("{} has {}") == " has ");
		assert(format("{} has {}", "John") == "John has ");
		assert(format("{} has {}", "John", 10) == "John has 10");
		assert(format("{} has {}", "John", 10, 20) == "John has 10");
	});
	test("toInt", []
	{
		assert(string::toInt<int>("1337", 0) == 1337);
		assert(string::toInt<int>("1337.", 0) == 1337);
		assert(string::toInt<int, string::TI_FULL>("1337", 0) == 1337);
		assert(string::toInt<int, string::TI_FULL>("1337.", 0) == 0);
		assert(string::hexToInt<int>("1337", 0) == 0x1337);
		assert(string::hexToInt<int>("1337.", 0) == 0x1337);
		assert(string::hexToInt<int, string::TI_FULL>("1337", 0) == 0x1337);
		assert(string::hexToInt<int, string::TI_FULL>("1337.", 0) == 0);
		assert(string::toInt<int>(L"1337", 0) == 1337);
		assert(string::toInt<int>(L"1337.", 0) == 1337);
		assert(string::toInt<int, string::TI_FULL>(L"1337", 0) == 1337);
		assert(string::toInt<int, string::TI_FULL>(L"1337.", 0) == 0);
		assert(string::hexToInt<int>(L"1337", 0) == 0x1337);
		assert(string::hexToInt<int>(L"1337.", 0) == 0x1337);
		assert(string::hexToInt<int, string::TI_FULL>(L"1337", 0) == 0x1337);
		assert(string::hexToInt<int, string::TI_FULL>(L"1337.", 0) == 0);
	});
}

static void unit_util()
{
	test("bin2hex", []
	{
		assert(string::bin2hex("\x1\x2\x3") == "010203");
	});
	test("version_compare", []
	{
		assert(version_compare("1.0", "0.1") > 0);
		assert(version_compare("0.1", "0") > 0);
	});
}

static void unit_vis()
{
	test("Rgb", []
	{
		assert(Rgb(0x00, 0x00, 0x00).toHex() == "000");
		assert(Rgb(0xFF, 0x00, 0xFF).toHex() == "F0F");
		assert(Rgb(0xFF, 0xFF, 0xFF).toHex() == "FFF");
		assert(Rgb(0x12, 0x34, 0x56).toHex() == "123456");
	});
}

void cli_test()
{
	unit("soup")
	{
		unit("cpu")
		{
			unit_cpu();
		}
		unit("crypto")
		{
			unit_crypto();
		}
		unit("data")
		{
			unit_data();
		}
		unit("io")
		{
			unit_io();
		}
		unit("lang")
		{
			unit_lang();
		}
		unit("ling")
		{
			unit("chatbot")
			{
				test("triggers", &test_chatbot_triggers);
				test("args", &test_chatbot_args);
				test("implementables", &test_chatbot_implementables);
			}
		}
		unit("math")
		{
			unit_math();
		}
		unit("net")
		{
			unit("web")
			{
				test("uri", &test_uri);
			}
			test("socket raii semantics", &test_socket_raii_semantics);
		}
		unit("util")
		{
			unit("string")
			{
				unit_util_string();
			}
			unit_util();
		}
		unit("vis")
		{
			unit_vis();
		}
	}
}
