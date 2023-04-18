#include "plist.hpp"

#include "base.hpp"
#include "Exception.hpp"
#include "format.hpp"
#include "xml.hpp"

namespace soup
{
	std::string PlistNode::toString(const std::string& prefix) const noexcept
	{
		std::string str;
		switch (type)
		{
		case DICT:
			for (const auto& child : static_cast<const PlistDict*>(this)->children)
			{
				str.append(prefix);
				str.append(child.first);
				str.push_back('\n');
				str.append(child.second->toString(std::string(prefix).append("\t")));
				str.push_back('\n');
			}
			if (!str.empty())
			{
				str.pop_back();
			}
			break;

		case ARRAY:
			{
				size_t i = 0;
				for (const auto& child : static_cast<const PlistArray*>(this)->children)
				{
					str.append(prefix);
					str.append(std::to_string(i++));
					str.push_back('\n');
					str.append(child->toString(std::string(prefix).append("\t")));
					str.push_back('\n');
				}
				if (!str.empty())
				{
					str.pop_back();
				}
			}
			break;

		case STRING:
			str = prefix;
			str.append(static_cast<const PlistString*>(this)->data);
			break;
		}
		return str;
	}

	bool PlistNode::isDict() const noexcept
	{
		return type == DICT;
	}

	bool PlistNode::isArray() const noexcept
	{
		return type == ARRAY;
	}

	bool PlistNode::isString() const noexcept
	{
		return type == STRING;
	}

	PlistDict& PlistNode::asDict()
	{
		if (!isDict())
		{
			throw Exception("PlistNode has unexpected type");
		}
		return *static_cast<PlistDict*>(this);
	}

	PlistArray& PlistNode::asArray()
	{
		if (!isArray())
		{
			throw Exception("PlistNode has unexpected type");
		}
		return *static_cast<PlistArray*>(this);
	}

	PlistString& PlistNode::asString()
	{
		if (!isString())
		{
			throw Exception("PlistNode has unexpected type");
		}
		return *static_cast<PlistString*>(this);
	}

	const PlistDict& PlistNode::asDict() const
	{
		if (!isDict())
		{
			throw Exception("PlistNode has unexpected type");
		}
		return *static_cast<const PlistDict*>(this);
	}

	const PlistArray& PlistNode::asArray() const
	{
		if (!isArray())
		{
			throw Exception("PlistNode has unexpected type");
		}
		return *static_cast<const PlistArray*>(this);
	}

	const PlistString& PlistNode::asString() const
	{
		if (!isString())
		{
			throw Exception("PlistNode has unexpected type");
		}
		return *static_cast<const PlistString*>(this);
	}

	std::vector<UniquePtr<PlistNode>> plist::parse(const XmlTag& tag)
	{
		std::vector<UniquePtr<PlistNode>> nodes{};
		for (const auto& child : tag.children)
		{
			if (!child->is_text)
			{
				nodes.emplace_back(parseTag(*static_cast<XmlTag*>(child.get())));
			}
		}
		return nodes;
	}

	UniquePtr<PlistNode> plist::parseTag(const XmlTag& tag)
	{
		if (tag.name == "dict")
		{
			auto dict = soup::make_unique<PlistDict>();
			std::string key{};
			for (const auto& child : tag.children)
			{
				if (child->is_text)
				{
					continue;
				}
				if (static_cast<XmlTag*>(child.get())->name == "key")
				{
					SOUP_IF_UNLIKELY (!key.empty())
					{
						throw Exception(format("Key given but no value provided: {}", key));
					}
					key = static_cast<XmlTag*>(child.get())->children.at(0)->asText().contents;
				}
				else
				{
					SOUP_IF_UNLIKELY (key.empty())
					{
						throw Exception(format("Expected key, found {}", static_cast<XmlTag*>(child.get())->name));
					}
					dict->children.emplace(std::move(key), parseTag(*static_cast<XmlTag*>(child.get())));
					key.clear();
				}
			}
			if (!key.empty())
			{
				throw Exception(format("Key given but no value provided: {}", key));
			}
			return dict;
		}
		else if (tag.name == "array")
		{
			auto arr = soup::make_unique<PlistArray>();
			for (const auto& child : tag.children)
			{
				if (!child->is_text)
				{
					arr->children.emplace_back(parseTag(*static_cast<XmlTag*>(child.get())));
				}
			}
			return arr;
		}
		else if (tag.name == "string")
		{
			auto str = soup::make_unique<PlistString>();
			str->data = tag.children.at(0)->asText().contents;
			return str;
		}
		else if (tag.name == "key")
		{
			throw Exception("Unexpected key");
		}
		throw Exception(format("Unknown type: {}", tag.name));
	}
}
