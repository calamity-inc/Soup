#include "JsonNode.hpp"

#include "branchless.hpp"
#include "JsonArray.hpp"
#include "JsonObject.hpp"

namespace soup
{
#if SOUP_SPACESHIP
	std::strong_ordering JsonNode::operator<=>(const JsonNode& b) const
	{
		if (type != b.type)
		{
			return type <=> b.type;
		}
		return encode() <=> b.encode();
	}
#endif

	bool JsonNode::operator==(const JsonNode& b) const
	{
		return type == b.type
			&& encode() == b.encode()
			;
	}

	bool JsonNode::operator!=(const JsonNode& b) const
	{
		return !operator==(b);
	}

	bool JsonNode::operator<(const JsonNode& b) const
	{
		return type < b.type
			|| encode() < b.encode()
			;
	}

	std::string JsonNode::encodePretty(const std::string& prefix) const
	{
		if (isArr())
		{
			return reinterpretAsArr().encodePretty(prefix);
		}
		if (isObj())
		{
			return reinterpretAsObj().encodePretty(prefix);
		}
		return encode();
	}

	bool JsonNode::isArr() const noexcept
	{
		return type == JSON_ARRAY;
	}

	bool JsonNode::isBool() const noexcept
	{
		return type == JSON_BOOL;
	}

	bool JsonNode::isFloat() const noexcept
	{
		return type == JSON_FLOAT;
	}

	bool JsonNode::isInt() const noexcept
	{
		return type == JSON_INT;
	}

	bool JsonNode::isNull() const noexcept
	{
		return type == JSON_NULL;
	}

	bool JsonNode::isObj() const noexcept
	{
		return type == JSON_OBJECT;
	}

	bool JsonNode::isStr() const noexcept
	{
		return type == JSON_STRING;
	}

	JsonArray* JsonNode::asArr() noexcept
	{
		return reinterpret_cast<JsonArray*>(branchless::trinary<JsonNode*>(isArr(), this, nullptr));
	}

	JsonBool* JsonNode::asBool() noexcept
	{
		return reinterpret_cast<JsonBool*>(branchless::trinary<JsonNode*>(isBool(), this, nullptr));
	}

	JsonFloat* JsonNode::asFloat() noexcept
	{
		return reinterpret_cast<JsonFloat*>(branchless::trinary<JsonNode*>(isFloat(), this, nullptr));
	}

	JsonInt* JsonNode::asInt() noexcept
	{
		return reinterpret_cast<JsonInt*>(branchless::trinary<JsonNode*>(isInt(), this, nullptr));
	}

	JsonObject* JsonNode::asObj() noexcept
	{
		return reinterpret_cast<JsonObject*>(branchless::trinary<JsonNode*>(isObj(), this, nullptr));
	}

	JsonString* JsonNode::asStr() noexcept
	{
		return reinterpret_cast<JsonString*>(branchless::trinary<JsonNode*>(isStr(), this, nullptr));
	}

	JsonArray& JsonNode::reinterpretAsArr() noexcept
	{
		return *reinterpret_cast<JsonArray*>(this);
	}

	JsonBool& JsonNode::reinterpretAsBool() noexcept
	{
		return *reinterpret_cast<JsonBool*>(this);
	}

	JsonFloat& JsonNode::reinterpretAsFloat() noexcept
	{
		return *reinterpret_cast<JsonFloat*>(this);
	}

	JsonInt& JsonNode::reinterpretAsInt() noexcept
	{
		return *reinterpret_cast<JsonInt*>(this);
	}

	JsonObject& JsonNode::reinterpretAsObj() noexcept
	{
		return *reinterpret_cast<JsonObject*>(this);
	}

	JsonString& JsonNode::reinterpretAsStr() noexcept
	{
		return *reinterpret_cast<JsonString*>(this);
	}

	const JsonArray& JsonNode::reinterpretAsArr() const noexcept
	{
		return *reinterpret_cast<const JsonArray*>(this);
	}

	const JsonBool& JsonNode::reinterpretAsBool() const noexcept
	{
		return *reinterpret_cast<const JsonBool*>(this);
	}

	const JsonFloat& JsonNode::reinterpretAsFloat() const noexcept
	{
		return *reinterpret_cast<const JsonFloat*>(this);
	}

	const JsonInt& JsonNode::reinterpretAsInt() const noexcept
	{
		return *reinterpret_cast<const JsonInt*>(this);
	}

	const JsonObject& JsonNode::reinterpretAsObj() const noexcept
	{
		return *reinterpret_cast<const JsonObject*>(this);
	}

	const JsonString& JsonNode::reinterpretAsStr() const noexcept
	{
		return *reinterpret_cast<const JsonString*>(this);
	}
}
