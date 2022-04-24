#include "JsonObject.hpp"

#include "Json.hpp"
#include "JsonInt.hpp"
#include "JsonString.hpp"
#include "string.hpp"

namespace soup
{
	JsonObject::JsonObject() noexcept
		: JsonNode(JSON_OBJECT)
	{
	}

	JsonObject::JsonObject(const char*& c) noexcept
		: JsonObject()
	{
		while (true)
		{
			while (string::isSpace(*c))
			{
				++c;
			}
			auto key = Json::decodeForDedicatedVariable(c);
			while (string::isSpace(*++c));
			auto val = Json::decodeForDedicatedVariable(c);
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

	std::string JsonObject::encode() const
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

	std::string JsonObject::encodePretty(const std::string& prefix) const
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

	bool JsonObject::contains(const JsonNode& k)
	{
		return children.contains(k);
	}

	bool JsonObject::contains(std::string k)
	{
		return contains(JsonString(std::move(k)));
	}

	JsonNode& JsonObject::at(const JsonNode& k)
	{
		return *children.at(k);
	}

	JsonNode& JsonObject::at(std::string k)
	{
		return at(JsonString(std::move(k)));
	}

	void JsonObject::add(UniquePtr<JsonNode>&& k, UniquePtr<JsonNode>&& v)
	{
		children.emplace(std::move(k), std::move(v));
	}

	void JsonObject::add(std::string k, std::string v)
	{
		add(soup::make_unique<JsonString>(std::move(k)), soup::make_unique<JsonString>(std::move(v)));
	}

	void JsonObject::add(std::string k, int64_t v)
	{
		add(soup::make_unique<JsonString>(std::move(k)), soup::make_unique<JsonInt>(std::move(v)));
	}
}
