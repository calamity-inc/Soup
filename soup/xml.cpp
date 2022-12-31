#include "xml.hpp"

#include "StringBuilder.hpp"
#include "UniquePtr.hpp"

//#include <iostream>

namespace soup
{
	std::string XmlTag::encode() const noexcept
	{
		std::string str(1, '<');
		str.append(name);
		for (const auto& e : attributes)
		{
			str.push_back(' ');
			//str.push_back('{');
			str.append(e.first);
			//str.push_back('}');
			if (!e.second.empty())
			{
				str.append("=\"");
				str.append(e.second);
				str.push_back('"');
			}
		}
		str.push_back('>');
		for (const auto& child : children)
		{
			if (child->is_text)
			{
				//str.push_back('{');
				str.append(reinterpret_cast<XmlText*>(child.get())->contents);
				//str.push_back('}');
			}
			else
			{
				str.append(reinterpret_cast<XmlTag*>(child.get())->encode());
			}
		}
		str.append("</");
		str.append(name);
		str.push_back('>');
		return str;
	}

	UniquePtr<XmlTag> xml::parse(const std::string& xml)
	{
		auto i = xml.begin();
		auto tag = parse(xml, i);
		if (i == xml.end()
			|| ++i == xml.end()
			)
		{
			return tag;
		}
		auto body = soup::make_unique<XmlTag>();
		body->name = "body";
		body->children.emplace_back(std::move(tag));
		do
		{
			body->children.emplace_back(parse(xml, i));
		} while (i != xml.end() && ++i != xml.end());
		return body;
	}

	UniquePtr<XmlTag> xml::parse(const std::string& xml, std::string::const_iterator& i)
	{
		auto tag = soup::make_unique<XmlTag>();
		if (*i == '<')
		{
			++i;
			StringBuilder name_builder;
			name_builder.beginCopy(xml, i);
			for (;; ++i)
			{
				if (i == xml.end())
				{
					return tag;
				}
				if (*i == ' ' || *i == '/' || *i == '>')
				{
					break;
				}
			}
			name_builder.endCopy(xml, i);
			tag->name = std::move(name_builder);
			if (*i != '>')
			{
				// Attributes
				StringBuilder name;
				name.beginCopy(xml, i);
				for (;; ++i)
				{
					if (i == xml.end())
					{
						return tag;
					}
					if (*i == '>')
					{
						name.endCopy(xml, i);
						if (!name.empty())
						{
							tag->attributes.emplace_back(std::move(name), std::string());
							name.clear();
						}
						break;
					}
					if (*i == ' ')
					{
						name.endCopy(xml, i);
						if (!name.empty())
						{
							tag->attributes.emplace_back(std::move(name), std::string());
							name.clear();
						}
						name.beginCopy(xml, i + 1);
					}
					else if (*i == '/')
					{
						name.endCopy(xml, i);
						if (!name.empty())
						{
							tag->attributes.emplace_back(std::move(name), std::string());
							name.clear();
						}
						if ((i + 1) != xml.end()
							&& *(i + 1) == '>'
							)
						{
							//std::cout << tag->name << " taking the easy way out" << std::endl;
							++i;
							return tag;
						}
						name.beginCopy(xml, i + 1);
					}
					else if (*i == '=')
					{
						name.endCopy(xml, i);
						StringBuilder value;
						++i;
						if (*i == '"')
						{
							//std::cout << "Collecting value for attribute " << name << ": ";
							++i;
							value.beginCopy(xml, i);
							for (;; ++i)
							{
								if (i == xml.end())
								{
									return tag;
								}
								if (*i == '"')
								{
									break;
								}
								//std::cout << *i;
							}
							//std::cout << std::endl;
							value.endCopy(xml, i);
							tag->attributes.emplace_back(std::move(name), std::move(value));
						}
						else
						{
							//std::cout << "Attribute " << name << " has no value";
							tag->attributes.emplace_back(std::move(name), std::string());
						}
						name.clear();
						name.beginCopy(xml, i + 1);
					}
				}
			}
			++i;
		}
		StringBuilder text;
		text.beginCopy(xml, i);
		for (;; ++i)
		{
			if (i == xml.end())
			{
				break;
			}
			if (*i == '<')
			{
				text.endCopy(xml, i);
				if (!text.empty())
				{
					//std::cout << "Copied text: " << text << std::endl;
					tag->children.emplace_back(soup::make_unique<XmlText>(std::move(text)));
					text.clear();
				}

				if ((i + 1) != xml.end()
					&& *(i + 1) == '/'
					)
				{
					i += 2;
					StringBuilder tbc_tag;
					tbc_tag.beginCopy(xml, i);
					for (;; ++i)
					{
						if (i == xml.end())
						{
							break;
						}
						if (*i == '>')
						{
							break;
						}
					}
					tbc_tag.endCopy(xml, i);
					//std::cout << "tbc tag: " << tbc_tag << std::endl;
					if (tbc_tag != tag->name)
					{
						//std::cout << "Expected tbc tag to be " << tag->name << std::endl;
						i -= tbc_tag.length();
						i -= 3;
						//std::cout << "Cursor now at " << *i << ", unwinding" << std::endl;
						return tag;
					}
					text.beginCopy(xml, i);
					break;
				}
				//auto child = parse(xml, i);
				//std::cout << "Recursed for " << child->name << ": " << child->encode() << std::endl;
				//tag->children.emplace_back(std::move(child));
				tag->children.emplace_back(parse(xml, i));
				if (i == xml.end())
				{
					break;
				}
				text.beginCopy(xml, i + 1);
			}
		}
		text.endCopy(xml, i);
		if (!text.empty())
		{
			//std::cout << "Copied text before return: " << text << std::endl;
			tag->children.emplace_back(soup::make_unique<XmlText>(std::move(text)));
			text.clear();
		}
		return tag;
	}
}
