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

	void lyoContainer::flattenElement(lyoFlatDocument& flat)
	{
		auto og_x = flat_x;
		auto og_y = flat_y;

		flat_x += margin_left;
		flat_y += margin_top;

		flat.elms.emplace_back(this);
		for (auto& elm : children)
		{
			elm->flattenElement(flat);
		}

		flat_x = og_x;
		flat_y = og_y;
	}
}
