#include "json_string.hpp"

namespace soup
{
	json_string::json_string() noexcept
		: json_node(JSON_STRING)
	{
	}

	json_string::json_string(std::string&& value) noexcept
		: json_node(JSON_STRING), value(std::move(value))
	{
	}

	json_string::json_string(const char*& c)
		: json_string()
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

	std::string json_string::encode() const
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
