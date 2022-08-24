#include "lyoContainer.hpp"

#include "lyoFlatDocument.hpp"
#include "lyoTextElement.hpp"
#include "unicode.hpp"

namespace soup
{
	lyoTextElement* lyoContainer::addText(const std::string& text)
	{
		return addText(unicode::utf8_to_utf32(text));
	}

	lyoTextElement* lyoContainer::addText(std::u32string text)
	{
		return reinterpret_cast<lyoTextElement*>(children.emplace_back(soup::make_unique<lyoTextElement>(this, std::move(text))).get());
	}

	lyoElement* lyoContainer::querySelector(const std::string& selector)
	{
		if (matchesSelector(selector))
		{
			return this;
		}
		for(const auto& node : children)
		{
			if (node->matchesSelector(selector))
			{
				return node.get();
			}
		}
		return nullptr;
	}

	std::vector<lyoElement*> lyoContainer::querySelectorAll(const std::string& selector)
	{
		std::vector<lyoElement*> res{};
		if (matchesSelector(selector))
		{
			res.emplace_back(this);
		}
		for (const auto& node : children)
		{
			if (node->matchesSelector(selector))
			{
				res.emplace_back(node.get());
			}
		}
		return res;
	}

	void lyoContainer::flattenElement(lyoFlatDocument& flat)
	{
		auto og_x = flat_x;
		auto og_y = flat_y;

		flat_x += style.margin_left;
		flat_y += style.margin_top;

		flat.elms.emplace_back(this);
		for (auto& elm : children)
		{
			elm->flattenElement(flat);
		}

		flat_x = og_x;
		flat_y = og_y;
	}
}
