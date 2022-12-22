#include "rflParser.hpp"

#include "LangDesc.hpp"
#include "rflFunc.hpp"
#include "rflStruct.hpp"
#include "rflType.hpp"
#include "rflVar.hpp"

namespace soup
{
	rflParser::rflParser(const std::string& code)
		: LexemeParser(LangDesc{}, code)
	{
	}

	rflType rflParser::readType()
	{
		rflType type;
		type.name = readLiteral();
		if (type.name == "const")
		{
			type.name.push_back(' ');
			type.name.append(readLiteral());
		}
		if (type.name == "unsigned")
		{
			type.name.push_back(' ');
			type.name.append(readLiteral());
		}
		type.at = rflType::DIRECT;
		if (type.name.back() == '*')
		{
			type.name.pop_back();
			type.at = rflType::POINTER;
		}
		else if (type.name.back() == '&')
		{
			type.name.pop_back();
			if (type.name.back() == '&')
			{
				type.name.pop_back();
				type.at = rflType::RVALUE_REFERENCE;
			}
			else
			{
				type.at = rflType::REFERENCE;
			}
		}
		return type;
	}

	rflVar rflParser::readVar()
	{
		rflVar var;
		readVar(var);
		return var;
	}

	void rflParser::readVar(rflVar& var)
	{
		var.type = readType();
		var.name = readLiteral();
	}

	rflFunc rflParser::readFunc()
	{
		rflFunc f;
		f.return_type = readType();
		f.name = readLiteral();
		if (readLiteral() != "(")
		{
			throw 0;
		}
		if (peekLiteral() != ")")
		{
			while (true)
			{
				readVar(f.parameters.emplace_back(rflVar{}));
				if (peekLiteral() == ",") // More parameters?
				{
					advance();
				}
				else if (readLiteral() == ")") // End of parameters?
				{
					break;
				}
				else // Wat?
				{
					throw 0;
				}
			}
		}
		return f;
	}

	rflStruct rflParser::readStruct()
	{
		rflMember::Accessibility accessibility = rflMember::PUBLIC; // struct & union
		auto st = readLiteral();
		if (st == "class")
		{
			accessibility = rflMember::PRIVATE;
		}
		else if (st != "struct")
		{
			throw 0;
		}
		rflStruct desc;
		desc.name = readLiteral();
		if (desc.name == "{")
		{
			desc.name.clear();
		}
		else
		{
			if (readLiteral() != "{")
			{
				throw 0;
			}
		}
		while (peekLiteral() != "}")
		{
			rflMember& member = desc.members.emplace_back(rflMember{});
			readVar(member);
			member.accessibility = accessibility;
		}
		advance(); // skip '}'
		return desc;
	}

	void rflParser::align()
	{
		while (i->isSpace()
			|| (
				i->isLiteral()
				&& (i->getLiteral() == "//"
					|| i->getLiteral().at(0) == '#'
					)
				)
			)
		{
			if (!i->isSpace())
			{
				do
				{
					advance();
				} while (!i->isNewLine());
			}
			advance();
		}
	}

	std::string rflParser::readLiteral()
	{
		align();
		if (!i->isLiteral())
		{
			throw 0;
		}
		return (i++)->val.getString();
	}

	std::string rflParser::peekLiteral()
	{
		align();
		if (i->isLiteral())
		{
			return i->val.getString();
		}
		return {};
	}
}
