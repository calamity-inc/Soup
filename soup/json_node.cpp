#include "json_node.hpp"

#include "json_array.hpp"
#include "json_object.hpp"
#include "optimised.hpp"

namespace soup
{
	std::strong_ordering json_node::operator<=>(const json_node& b) const
	{
		if (type != b.type)
		{
			return type <=> b.type;
		}
		return encode() <=> b.encode();
	}

	bool json_node::operator==(const json_node& b) const
	{
		return type == b.type
			&& encode() == b.encode()
			;
	}

	bool json_node::operator!=(const json_node& b) const
	{
		return !operator==(b);
	}

	bool json_node::operator<(const json_node& b) const
	{
		return type < b.type
			|| encode() < b.encode()
			;
	}

	std::string json_node::encodePretty(const std::string& prefix) const
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

	bool json_node::isArr() const noexcept
	{
		return type == JSON_ARRAY;
	}

	bool json_node::isBool() const noexcept
	{
		return type == JSON_BOOL;
	}

	bool json_node::isFloat() const noexcept
	{
		return type == JSON_FLOAT;
	}

	bool json_node::isInt() const noexcept
	{
		return type == JSON_INT;
	}

	bool json_node::isNull() const noexcept
	{
		return type == JSON_NULL;
	}

	bool json_node::isObj() const noexcept
	{
		return type == JSON_OBJECT;
	}

	bool json_node::isStr() const noexcept
	{
		return type == JSON_STRING;
	}

	json_array* json_node::asArr() noexcept
	{
		return reinterpret_cast<json_array*>(optimised::trinary<json_node*>(isArr(), this, nullptr));
	}

	json_bool* json_node::asBool() noexcept
	{
		return reinterpret_cast<json_bool*>(optimised::trinary<json_node*>(isBool(), this, nullptr));
	}

	json_float* json_node::asFloat() noexcept
	{
		return reinterpret_cast<json_float*>(optimised::trinary<json_node*>(isFloat(), this, nullptr));
	}

	json_int* json_node::asInt() noexcept
	{
		return reinterpret_cast<json_int*>(optimised::trinary<json_node*>(isInt(), this, nullptr));
	}

	json_object* json_node::asObj() noexcept
	{
		return reinterpret_cast<json_object*>(optimised::trinary<json_node*>(isObj(), this, nullptr));
	}

	json_string* json_node::asStr() noexcept
	{
		return reinterpret_cast<json_string*>(optimised::trinary<json_node*>(isStr(), this, nullptr));
	}

	json_array& json_node::reinterpretAsArr() noexcept
	{
		return *reinterpret_cast<json_array*>(this);
	}

	json_bool& json_node::reinterpretAsBool() noexcept
	{
		return *reinterpret_cast<json_bool*>(this);
	}

	json_float& json_node::reinterpretAsFloat() noexcept
	{
		return *reinterpret_cast<json_float*>(this);
	}

	json_int& json_node::reinterpretAsInt() noexcept
	{
		return *reinterpret_cast<json_int*>(this);
	}

	json_object& json_node::reinterpretAsObj() noexcept
	{
		return *reinterpret_cast<json_object*>(this);
	}

	json_string& json_node::reinterpretAsStr() noexcept
	{
		return *reinterpret_cast<json_string*>(this);
	}
	
	const json_array& json_node::reinterpretAsArr() const noexcept
	{
		return *reinterpret_cast<const json_array*>(this);
	}

	const json_bool& json_node::reinterpretAsBool() const noexcept
	{
		return *reinterpret_cast<const json_bool*>(this);
	}

	const json_float& json_node::reinterpretAsFloat() const noexcept
	{
		return *reinterpret_cast<const json_float*>(this);
	}

	const json_int& json_node::reinterpretAsInt() const noexcept
	{
		return *reinterpret_cast<const json_int*>(this);
	}

	const json_object& json_node::reinterpretAsObj() const noexcept
	{
		return *reinterpret_cast<const json_object*>(this);
	}

	const json_string& json_node::reinterpretAsStr() const noexcept
	{
		return *reinterpret_cast<const json_string*>(this);
	}
}
