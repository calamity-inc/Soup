#include "json.hpp"

#include "json_array.hpp"
#include "json_bool.hpp"
#include "json_float.hpp"
#include "json_int.hpp"
#include "json_null.hpp"
#include "json_object.hpp"
#include "json_string.hpp"

namespace soup
{
	void json::decode(unique_ptr<json_node>& out, const std::string& data)
	{
		out = decodeForDedicatedVariable(data);
	}

	void json::decode(unique_ptr<json_node>& out, const char*& c)
	{
		out = decodeForDedicatedVariable(c);
	}

	unique_ptr<json_node> json::decodeForDedicatedVariable(const std::string& data)
	{
		if (data.empty())
		{
			return {};
		}
		const char* c = &data.at(0);
		return decodeForDedicatedVariable(c);
	}

	unique_ptr<json_node> json::decodeForDedicatedVariable(const char*& c)
	{
		switch (*c)
		{
		case '"':
			++c;
			return soup::make_unique<json_string>(c);

		case '[':
			++c;
			return soup::make_unique<json_array>(c);

		case '{':
			++c;
			return soup::make_unique<json_object>(c);
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
					return soup::make_unique<json_int>(opt.value());
				}
			}
			else if (is_float)
			{
				try
				{
					return soup::make_unique<json_float>(std::stod(buf));
				}
				catch (...)
				{
				}
			}
			else if (buf == "true")
			{
				return soup::make_unique<json_bool>(true);
			}
			else if (buf == "false")
			{
				return soup::make_unique<json_bool>(false);
			}
			else if (buf == "null")
			{
				return soup::make_unique<json_null>();
			}
		}
		return {};
	}
}
