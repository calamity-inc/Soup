#include "JsonInt.hpp"

#include "Writer.hpp"

NAMESPACE_SOUP
{
	JsonInt::JsonInt(int64_t value) noexcept
		: JsonNode(JSON_INT), value(value)
	{
	}

	bool JsonInt::operator==(const JsonNode& b) const noexcept
	{
		return JSON_INT == b.type
			&& value == b.reinterpretAsInt().value
			;
	}

	void JsonInt::encodeAndAppendTo(std::string& str) const SOUP_EXCAL
	{
		str.append(std::to_string(value));
	}

	bool JsonInt::binaryEncode(Writer& w) const
	{
		uint8_t b = JSON_INT;
		if (value >= 0 && value < 0b11111)
		{
			b |= (value << 3);
			return w.u8(b);
		}
		b |= (0b11111 << 3);
		bool ret = w.u8(b);
		ret &= w.i64_dyn(value);
		return ret;
	}

	bool JsonInt::binaryEncodeV2(Writer& w) const
	{
		bool ret;

		uint64_t u;
		bool neg = (value < 0);
		if (neg)
		{
			u = (value * -1) - 1;
		}
		else
		{
			u = value;
		}

		// 2 type bits (incl. 1 sign bit), 5 value bits, 1 'more' bit
		uint8_t b = 2 + neg;
		b |= (u & 0b11111) << 2;
		u >>= 5;
		if (u)
		{
			b |= 0x80;
			ret = w.u8(b);
			ret &= w.u64_dyn_v2(u);
		}
		else
		{
			ret = w.u8(b);
		}
		return ret;
	}
}
