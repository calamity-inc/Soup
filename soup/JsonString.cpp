#include "JsonString.hpp"

namespace soup
{
	JsonString::JsonString() noexcept
		: JsonNode(JSON_STRING)
	{
	}

	JsonString::JsonString(std::string&& value) noexcept
		: JsonNode(JSON_STRING), value(std::move(value))
	{
	}

	JsonString::JsonString(const char*& c)
		: JsonString()
	{
		for (bool escaped = false; *c != 0; ++c)
		{
			if (escaped)
			{
				escaped = false;
				switch (*c)
				{
				default:
					value.push_back(*c);
					break;

				case 'n':
					value.push_back('\n');
					break;

				case 'r':
					value.push_back('\r');
					break;
				}
				continue;
			}
			if (*c == '"')
			{
				++c;
				break;
			}
			if (*c == '\\')
			{
				escaped = true;
				continue;
			}
			value.push_back(*c);
		}
	}

	std::string JsonString::encode() const
	{
		std::string str{ value };
		string::replace_all(str, "\\", "\\\\");
		string::replace_all(str, "\"", "\\\"");
		string::replace_all(str, "\r", "\\r");
		string::replace_all(str, "\n", "\\n");
		str.insert(0, 1, '"');
		str.push_back('"');
		return str;
	}
}
