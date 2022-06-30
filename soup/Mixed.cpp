#include "Mixed.hpp"

#include <ostream>
#include <typeinfo> // bad_cast

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

		case BLOCK:
			throw std::runtime_error("Can't copy a block");
		}
	}

	Mixed::Mixed(Block* val)
		: type(BLOCK), val((uint64_t)val)
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

		case BLOCK:
			delete reinterpret_cast<Block*>(val);
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

		case BLOCK:
			return "block";
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
		if (type == BLOCK)
		{
			return reinterpret_cast<Block*>(val)->toString(prefix);
		}
		return {};
	}

	int64_t Mixed::getInt() const
	{
		if (type != INT)
		{
			throw std::bad_cast();
		}
		return (int64_t)val;
	}

	uint64_t Mixed::getUInt() const
	{
		if (type != UINT)
		{
			throw std::bad_cast();
		}
		return val;
	}

	std::string& Mixed::getString() const
	{
		if (type != STRING)
		{
			throw std::bad_cast();
		}
		return *reinterpret_cast<std::string*>(val);
	}

	std::string& Mixed::getFunc() const
	{
		if (type != FUNC)
		{
			throw std::bad_cast();
		}
		return *reinterpret_cast<std::string*>(val);
	}

	std::string& Mixed::getVarName() const
	{
		if (type != VAR_NAME)
		{
			throw std::bad_cast();
		}
		return *reinterpret_cast<std::string*>(val);
	}

	std::unordered_map<Mixed, std::shared_ptr<Mixed>>& Mixed::getMixedSpMixedMap() const
	{
		if (type != MIXED_SP_MIXED_MAP)
		{
			throw std::bad_cast();
		}
		return *reinterpret_cast<std::unordered_map<Mixed, std::shared_ptr<Mixed>>*>(val);
	}

	Block& Mixed::getBlock() const
	{
		if (type != BLOCK)
		{
			throw std::bad_cast();
		}
		return *reinterpret_cast<Block*>(val);
	}
}
