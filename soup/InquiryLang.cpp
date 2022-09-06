#include "InquiryLang.hpp"

#include "LangDesc.hpp"
#include "LangVm.hpp"
#include "Lexeme.hpp"
#include "ParserState.hpp"
#include "parse_tree.hpp"
#include "StringReader.hpp"
#include "StringWriter.hpp"

#include "base32.hpp"
#include "base64.hpp"
#include "adler32.hpp"
#include "crc32.hpp"
#include "joaat.hpp"
#include "ripemd160.hpp"
#include "sha1.hpp"
#include "sha256.hpp"

namespace soup
{
	enum InquireOps
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

	std::shared_ptr<Mixed> InquiryLang::execute(const std::string& q)
	{
		LangDesc ld;
		ld.addToken("base32_encode", &keywordParse<OP_BASE32_ENCODE>);
		ld.addToken("base32_decode", &keywordParse<OP_BASE32_DECODE>);
		ld.addToken("base64_encode", &keywordParse<OP_BASE64_ENCODE>);
		ld.addToken("base64_decode", &keywordParse<OP_BASE64_DECODE>);
		ld.addToken("adler32", &keywordParse<OP_ADLER32>);
		ld.addToken("crc32", &keywordParse<OP_CRC32>);
		ld.addToken("joaat", &keywordParse<OP_JOAAT>);
		ld.addToken("ripemd160", &keywordParse<OP_RIPEMD160>);
		ld.addToken("sha1", &keywordParse<OP_SHA1>);
		ld.addToken("sha256", &keywordParse<OP_SHA1>);
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
			}
		}
		if (!vm.stack.empty())
		{
			return vm.pop();
		}
		return {};
	}

	std::string InquiryLang::formatResult(std::shared_ptr<Mixed> res)
	{
		if (!res)
		{
			return "[no result]";
		}
		std::string str = res->toString();
		if (res->isUInt())
		{
			str.append(" / 0x");
			str.append(string::hex(res->getUInt()));
		}
		return str;
	}
}
