#include "Mixed.hpp"

#include <ostream>

#include "Exception.hpp"
#include "parse_tree.hpp"

namespace soup
{
	Mixed::Mixed(const Mixed& b)
		: type(b.type)
	{
		switch (type)
		{
		case NONE:
			break;

		case INT:
		case UINT:
			val = b.val;
			break;

		case STRING:
		case FUNC:
		case VAR_NAME:
			val = (uint64_t)new std::string(*reinterpret_cast<std::string*>(b.val));
			break;

		case MIXED_SP_MIXED_MAP:
			val = (uint64_t)new std::unordered_map<Mixed, std::shared_ptr<Mixed>>(b.getMixedSpMixedMap());
			break;

		case AST_BLOCK:
			throw Exception("Can't copy this type");
		}
	}

	Mixed::Mixed(astBlock* val)
		: type(AST_BLOCK), val((uint64_t)val)
	{
	}

	Mixed::Mixed(std::unordered_map<Mixed, std::shared_ptr<Mixed>>&& val)
		: type(MIXED_SP_MIXED_MAP), val((uint64_t)new std::unordered_map<Mixed, std::shared_ptr<Mixed>>(std::move(val)))
	{
	}

	void Mixed::release()
	{
		switch (type)
		{
		case NONE:
		case INT:
		case UINT:
			break;

		case STRING:
		case FUNC:
		case VAR_NAME:
			delete reinterpret_cast<std::string*>(val);
			break;

		case MIXED_SP_MIXED_MAP:
			delete reinterpret_cast<std::unordered_map<Mixed, std::shared_ptr<Mixed>>*>(val);
			break;

		case AST_BLOCK:
			delete reinterpret_cast<astBlock*>(val);
			break;
		}
	}

	std::ostream& operator<<(std::ostream& os, const Mixed& v)
	{
		os << v.toString();
		return os;
	}

	const char* Mixed::getTypeName(Type t) noexcept
	{
		switch (t)
		{
		default:
			break;

		case INT:
			return "int";

		case UINT:
			return "uint";

		case STRING:
			return "string";

		case FUNC:
			return "func";

		case VAR_NAME:
			return "var name";

		case AST_BLOCK:
			return "ast block";
		}
		return "complex type";
	}

	const char* Mixed::getTypeName() const noexcept
	{
		return getTypeName(type);
	}

	std::string Mixed::toString(const std::string& ast_block_prefix) const noexcept
	{
		if (type == INT)
		{
			return std::to_string((int64_t)val);
		}
		if (type == UINT)
		{
			return std::to_string(val);
		}
		if (type == STRING)
		{
			return *reinterpret_cast<std::string*>(val);
		}
		if (type == AST_BLOCK)
		{
			return reinterpret_cast<astBlock*>(val)->toString(ast_block_prefix);
		}
		return {};
	}

	std::string Mixed::toStringWithFallback() const noexcept
	{
		if (auto str = toString(); !str.empty())
		{
			return str;
		}
		std::string str(1, '[');
		str.append(getTypeName());
		str.push_back(']');
		return str;
	}

	void Mixed::assertType(Type e) const
	{
		SOUP_IF_UNLIKELY (type != e)
		{
			std::string str = "Expected Mixed to hold ";
			str.append(getTypeName(e));
			str.append(", found ");
			str.append(getTypeName());
			throw Exception(std::move(str));
		}
	}

	int64_t Mixed::getInt() const
	{
		assertType(INT);
		return (int64_t)val;
	}

	uint64_t Mixed::getUInt() const
	{
		assertType(UINT);
		return val;
	}

	std::string& Mixed::getString() const
	{
		assertType(STRING);
		return *reinterpret_cast<std::string*>(val);
	}

	std::string& Mixed::getFunc() const
	{
		assertType(FUNC);
		return *reinterpret_cast<std::string*>(val);
	}

	std::string& Mixed::getVarName() const
	{
		assertType(VAR_NAME);
		return *reinterpret_cast<std::string*>(val);
	}

	std::unordered_map<Mixed, std::shared_ptr<Mixed>>& Mixed::getMixedSpMixedMap() const
	{
		assertType(MIXED_SP_MIXED_MAP);
		return *reinterpret_cast<std::unordered_map<Mixed, std::shared_ptr<Mixed>>*>(val);
	}

	astBlock& Mixed::getAstBlock() const
	{
		assertType(AST_BLOCK);
		return *reinterpret_cast<astBlock*>(val);
	}
}
