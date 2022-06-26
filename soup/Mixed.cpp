#include "Mixed.hpp"

#include <ostream>

#include "Op.hpp"

namespace soup
{
	Mixed::Mixed(const Mixed& b)
		: type(b.type)
	{
		switch (type)
		{
		default:
			val = b.val;
			break;

		case STRING:
			val = (uint64_t)new std::string(b.getString());
			break;

		case OP_ARRAY:
			val = (uint64_t)new std::vector<Op>(b.getOpArray());
			break;
		}
	}

	Mixed::Mixed(std::vector<Op>&& val)
		: type(OP_ARRAY), val((uint64_t)new std::vector<Op>(std::move(val)))
	{
	}

	Mixed::Mixed(std::unordered_map<Mixed, Mixed>&& val)
		: type(MIXED_MIXED_MAP), val((uint64_t)new std::unordered_map<Mixed, Mixed>(std::move(val)))
	{
	}

	void Mixed::release()
	{
		switch (type)
		{
		default:
			break;

		case STRING:
			delete reinterpret_cast<std::string*>(val);
			break;

		case OP_ARRAY:
			delete reinterpret_cast<std::vector<Op>*>(val);
			break;

		case MIXED_MIXED_MAP:
			delete reinterpret_cast<std::unordered_map<Mixed, Mixed>*>(val);
			break;
		}
	}

	std::ostream& operator<<(std::ostream& os, const Mixed& v)
	{
		os << v.toString();
		return os;
	}

	int64_t Mixed::getInt() const
	{
		if (type != INT)
		{
			throw 0;
		}
		return (int64_t)val;
	}

	uint64_t Mixed::getUInt() const
	{
		if (type != UINT)
		{
			throw 0;
		}
		return val;
	}

	std::string& Mixed::getString() const
	{
		if (type != STRING)
		{
			throw 0;
		}
		return *reinterpret_cast<std::string*>(val);
	}

	std::vector<Op>& Mixed::getOpArray() const
	{
		if (type != OP_ARRAY)
		{
			throw 0;
		}
		return *reinterpret_cast<std::vector<Op>*>(val);
	}

	std::unordered_map<Mixed, Mixed>& Mixed::getMixedMixedMap() const
	{
		if (type != MIXED_MIXED_MAP)
		{
			throw 0;
		}
		return *reinterpret_cast<std::unordered_map<Mixed, Mixed>*>(val);
	}
}
