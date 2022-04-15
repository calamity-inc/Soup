#include "json_array.hpp"

#include "json.hpp"
#include "string.hpp"

namespace soup
{
	json_array::json_array() noexcept
		: json_node(JSON_ARRAY)
	{
	}

	json_array::json_array(const char*& c)
		: json_array()
	{
		while (true)
		{
			while (string::isSpace(*c))
			{
				++c;
			}
			auto val = json::decodeForDedicatedVariable(c);
			if (!val)
			{
				break;
			}
			children.emplace_back(std::move(val));
			while (*c == ',' || string::isSpace(*c))
			{
				++c;
			}
			if (*c == ']' || *c == 0)
			{
				break;
			}
		}
		++c;
	}

	std::string json_array::encode() const
	{
		std::string res(1, '[');
		for (auto i = children.begin(); i != children.end(); ++i)
		{
			res.append((*i)->encode());
			if (i != children.end() - 1)
			{
				res.append(", ");
			}
		}
		res.push_back(']');
		return res;
	}

	std::string json_array::encodePretty(const std::string& prefix) const
	{
		if (children.empty())
		{
			return "[]";
		}
		std::string rprefix = prefix;
		rprefix.append("    ");
		std::string res = "[\n";
		for (auto i = children.begin(); i != children.end(); ++i)
		{
			res.append(rprefix);
			res.append((*i)->encodePretty(rprefix));
			if (i != children.end() - 1)
			{
				res.push_back(',');
			}
			res.push_back('\n');
		}
		res.append(prefix).push_back(']');
		return res;
	}

	json_node& json_array::at(size_t i)
	{
		return *children.at(i);
	}
}
