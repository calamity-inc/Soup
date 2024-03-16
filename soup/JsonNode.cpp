#include "JsonNode.hpp"

#include "Exception.hpp"
#include "JsonArray.hpp"
#include "JsonObject.hpp"

namespace soup
{
#if SOUP_CPP20
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

	void JsonNode::encodePrettyAndAppendTo(std::string& str, const std::string& prefix) const SOUP_EXCAL
	{
		if (isArr())
		{
			reinterpretAsArr().encodePrettyAndAppendTo(str, prefix);
		}
		else if (isObj())
		{
			reinterpretAsObj().encodePrettyAndAppendTo(str, prefix);
		}
		else
		{
			encodeAndAppendTo(str);
		}
	}

	void JsonNode::throwTypeError()
	{
		SOUP_THROW(Exception("JsonNode has unexpected type"));
	}
}
