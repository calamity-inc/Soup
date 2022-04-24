#include "Json.hpp"

#include "JsonArray.hpp"
#include "JsonBool.hpp"
#include "JsonFloat.hpp"
#include "JsonInt.hpp"
#include "JsonNull.hpp"
#include "JsonObject.hpp"
#include "JsonString.hpp"

namespace soup
{
	void Json::decode(UniquePtr<JsonNode>& out, const std::string& data)
	{
		out = decodeForDedicatedVariable(data);
	}

	void Json::decode(UniquePtr<JsonNode>& out, const char*& c)
	{
		out = decodeForDedicatedVariable(c);
	}

	UniquePtr<JsonNode> Json::decodeForDedicatedVariable(const std::string& data)
	{
		if (data.empty())
		{
			return {};
		}
		const char* c = &data.at(0);
		return decodeForDedicatedVariable(c);
	}

	UniquePtr<JsonNode> Json::decodeForDedicatedVariable(const char*& c)
	{
		switch (*c)
		{
		case '"':
			++c;
			return soup::make_unique<JsonString>(c);

		case '[':
			++c;
			return soup::make_unique<JsonArray>(c);

		case '{':
			++c;
			return soup::make_unique<JsonObject>(c);
		}

		std::string buf{};
		bool is_int = true;
		bool is_float = false;
		for (; *c != ',' && !string::isSpace(*c) && *c != '}' && *c != ']' && *c != 0; ++c)
		{
			buf.push_back(*c);

			if (!string::isNumberChar(*c) && *c != '-')
			{
				is_int = false;
				is_float = (*c == '.');
			}
		}
		if (!buf.empty())
		{
			if (is_int)
			{
				auto opt = string::toInt<int64_t>(buf);
				if (opt.has_value())
				{
					return soup::make_unique<JsonInt>(opt.value());
				}
			}
			else if (is_float)
			{
				try
				{
					return soup::make_unique<JsonFloat>(std::stod(buf));
				}
				catch (...)
				{
				}
			}
			else if (buf == "true")
			{
				return soup::make_unique<JsonBool>(true);
			}
			else if (buf == "false")
			{
				return soup::make_unique<JsonBool>(false);
			}
			else if (buf == "null")
			{
				return soup::make_unique<JsonNull>();
			}
		}
		return {};
	}
}
