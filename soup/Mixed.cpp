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
			throw std::runtime_error("Can't copy this type");
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

	const char* Mixed::getTypeName() const noexcept
	{
		switch (type)
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

	std::string Mixed::toString(const std::string& prefix) const noexcept
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
			return reinterpret_cast<astBlock*>(val)->toString(prefix);
		}
		return {};
	}

	int64_t Mixed::getInt() const
	{
		if (type != INT)
		{
			throw Exception("Mixed has unexpected type");
		}
		return (int64_t)val;
	}

	uint64_t Mixed::getUInt() const
	{
		if (type != UINT)
		{
			throw Exception("Mixed has unexpected type");
		}
		return val;
	}

	std::string& Mixed::getString() const
	{
		if (type != STRING)
		{
			throw Exception("Mixed has unexpected type");
		}
		return *reinterpret_cast<std::string*>(val);
	}

	std::string& Mixed::getFunc() const
	{
		if (type != FUNC)
		{
			throw Exception("Mixed has unexpected type");
		}
		return *reinterpret_cast<std::string*>(val);
	}

	std::string& Mixed::getVarName() const
	{
		if (type != VAR_NAME)
		{
			throw Exception("Mixed has unexpected type");
		}
		return *reinterpret_cast<std::string*>(val);
	}

	std::unordered_map<Mixed, std::shared_ptr<Mixed>>& Mixed::getMixedSpMixedMap() const
	{
		if (type != MIXED_SP_MIXED_MAP)
		{
			throw Exception("Mixed has unexpected type");
		}
		return *reinterpret_cast<std::unordered_map<Mixed, std::shared_ptr<Mixed>>*>(val);
	}

	astBlock& Mixed::getAstBlock() const
	{
		if (type != AST_BLOCK)
		{
			throw Exception("Mixed has unexpected type");
		}
		return *reinterpret_cast<astBlock*>(val);
	}
}
