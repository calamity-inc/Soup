#include "InquiryLang.hpp"

#include "Canvas.hpp"
#include "LangDesc.hpp"
#include "LangVm.hpp"
#include "Lexeme.hpp"
#include "ParserState.hpp"
#include "parse_tree.hpp"
#include "StringReader.hpp"
#include "StringWriter.hpp"
#include "unicode.hpp"

#include "base32.hpp"
#include "base64.hpp"

#include "adler32.hpp"
#include "crc32.hpp"
#include "joaat.hpp"
#include "ripemd160.hpp"
#include "sha1.hpp"
#include "sha256.hpp"

#include "QrCode.hpp"

namespace soup
{
	enum InquiryOp
	{
		OP_BASE32_ENCODE,
		OP_BASE32_DECODE,
		OP_BASE64_ENCODE,
		OP_BASE64_DECODE,

		OP_ADLER32,
		OP_CRC32,
		OP_JOAAT,
		OP_RIPEMD160,
		OP_SHA1,
		OP_SHA256,

		OP_QRCODE,

		OP_HELP,
	};

	static void keywordConsume(ParserState& ps)
	{
		auto node = ps.popRighthand();
		if (!node->isValue())
		{
			ps.throwExpectedRighthandValue(node);
		}

		// Support literal strings
		if (node->type == astNode::LEXEME
			&& reinterpret_cast<LexemeNode*>(node.get())->lexeme.token_keyword == Lexeme::LITERAL
			)
		{
			ps.pushArg(std::move(reinterpret_cast<LexemeNode*>(node.get())->lexeme.val));
			return;
		}

		ps.pushArgNode(std::move(node));
	}

	template <int Op>
	static void keywordParse(ParserState& ps)
	{
		ps.setOp(Op);
		keywordConsume(ps);
	}

#define FOR_EACH_KEYWORD(region, keyword) \
region("data.enc"); \
keyword("base32_encode", OP_BASE32_ENCODE); \
keyword("base32_decode", OP_BASE32_DECODE); \
keyword("base64_encode", OP_BASE64_ENCODE); \
keyword("base64_decode", OP_BASE64_DECODE); \
region("data.hash"); \
keyword("adler32", OP_ADLER32); \
keyword("crc32", OP_CRC32); \
keyword("joaat", OP_JOAAT); \
keyword("ripemd160", OP_RIPEMD160); \
keyword("sha1", OP_SHA1); \
keyword("sha256", OP_SHA256); \
region("vis"); \
keyword("qrcode", OP_QRCODE); \

#define REGISTER_REGION(region)
#define REGISTER_KEYWORD(keyword, op) ld.addToken(keyword, &keywordParse<op>);

#define HELP_REGION(region) help.append(region).push_back('\n');
#define HELP_KEYWORD(keyword, op) help.append("- ").append(keyword).push_back('\n');

	SharedPtr<Mixed> InquiryLang::execute(const std::string& q)
	{
		LangDesc ld;
		FOR_EACH_KEYWORD(REGISTER_REGION, REGISTER_KEYWORD);
		ld.addToken("help", [](ParserState& ps)
		{
			ps.setOp(OP_HELP);
		});
		auto tks = ld.tokenise(q);
		auto ast = ld.parse(tks);
		//std::cout << ast.toString() << std::endl;
		StringWriter sw;
		ast.compile(sw);
		StringReader sr(std::move(sw.str));
		LangVm vm{ sr };
		for (uint8_t op; vm.getNextOp(op); )
		{
			switch (op)
			{
			case OP_BASE32_ENCODE:
				vm.push(base32::encode(vm.pop()->getString()));
				break;

			case OP_BASE32_DECODE:
				vm.push(base32::decode(vm.pop()->getString()));
				break;

			case OP_BASE64_ENCODE:
				vm.push(base64::encode(vm.pop()->getString()));
				break;

			case OP_BASE64_DECODE:
				vm.push(base64::decode(vm.pop()->getString()));
				break;

			case OP_ADLER32:
				vm.push(adler32::hash(vm.pop()->getString()));
				break;

			case OP_CRC32:
				vm.push(crc32::hash(vm.pop()->getString()));
				break;
			
			case OP_JOAAT:
				vm.push(joaat::hash(vm.pop()->getString()));
				break;

			case OP_RIPEMD160:
				vm.push(string::bin2hex(ripemd160(vm.pop()->getString())));
				break;

			case OP_SHA1:
				vm.push(string::bin2hex(sha1::hash(vm.pop()->getString())));
				break;

			case OP_SHA256:
				vm.push(string::bin2hex(sha256::hash(vm.pop()->getString())));
				break;

			case OP_QRCODE:
				vm.push(QrCode::encodeText(vm.pop()->getString()).toCanvas(4, true));
				break;

			case OP_HELP:
				{
					std::string help;
					FOR_EACH_KEYWORD(HELP_REGION, HELP_KEYWORD);
					help.erase(help.size() - 1);
					vm.push(std::move(help));
				}
				break;
			}
		}
		if (!vm.stack.empty())
		{
			return vm.pop();
		}
		return {};
	}

	std::string InquiryLang::formatResult(SharedPtr<Mixed> res)
	{
		SOUP_IF_UNLIKELY (!res)
		{
			return "[no result]";
		}
		return formatResult(*res);
	}

	std::string InquiryLang::formatResult(const Mixed& res)
	{
		if (res.isCanvas())
		{
			return unicode::utf16_to_utf8(res.getCanvas().toStringDownsampledDoublewidth(true));
		}
		auto str = formatResultLine(res);
		str.push_back('\n');
		return str;
	}

	std::string InquiryLang::formatResultLine(const Mixed& res)
	{
		std::string str = res.toStringWithFallback();
		if (res.isUInt())
		{
			str.append(" / 0x");
			str.append(string::hex(res.getUInt()));
		}
		return str;
	}
}
