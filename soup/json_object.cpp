#include "json_object.hpp"

#include "json.hpp"
#include "json_int.hpp"
#include "json_string.hpp"
#include "string.hpp"

namespace soup
{
	json_object::json_object() noexcept
		: json_node(JSON_OBJECT)
	{
	}

	json_object::json_object(const char*& c) noexcept
		: json_object()
	{
		while (true)
		{
			while (string::isSpace(*c))
			{
				++c;
			}
			auto key = json::decodeForDedicatedVariable(c);
			while (string::isSpace(*++c));
			auto val = json::decodeForDedicatedVariable(c);
			if (!key || !val)
			{
				break;
			}
			children.emplace(std::move(key), std::move(val));
			while (*c == ',' || string::isSpace(*c))
			{
				++c;
			}
			if (*c == '}' || *c == 0)
			{
				break;
			}
		}
		++c;
		children.ensureSorted();
	}

	std::string json_object::encode() const
	{
		std::string res(1, '{');
		for (auto i = children.begin(); i != children.end(); ++i)
		{
			res.append(i->first->encode());
			res.append(": ");
			res.append(i->second->encode());
			if (i != children.end() - 1)
			{
				res.append(", ");
			}
		}
		res.push_back('}');
		return res;
	}

	std::string json_object::encodePretty(const std::string& prefix) const
	{
		if (children.empty())
		{
			return "{}";
		}
		std::string rprefix = prefix;
		rprefix.append("    ");
		std::string res = "{\n";
		for (auto i = children.begin(); i != children.end(); ++i)
		{
			res.append(rprefix);
			res.append(i->first->encode());
			res.append(": ");
			res.append(i->second->encodePretty(rprefix));
			if (i != children.end() - 1)
			{
				res.push_back(',');
			}
			res.push_back('\n');
		}
		res.append(prefix).push_back('}');
		return res;
	}

	bool json_object::contains(const json_node& k)
	{
		return children.contains(k);
	}

	bool json_object::contains(std::string k)
	{
		return contains(json_string(std::move(k)));
	}

	json_node& json_object::at(const json_node& k)
	{
		return *children.at(k);
	}

	json_node& json_object::at(std::string k)
	{
		return at(json_string(std::move(k)));
	}

	void json_object::add(unique_ptr<json_node>&& k, unique_ptr<json_node>&& v)
	{
		children.emplace(std::move(k), std::move(v));
	}

	void json_object::add(std::string k, std::string v)
	{
		add(make_unique<json_string>(std::move(k)), make_unique<json_string>(std::move(v)));
	}

	void json_object::add(std::string k, int64_t v)
	{
		add(make_unique<json_string>(std::move(k)), make_unique<json_int>(std::move(v)));
	}
}
