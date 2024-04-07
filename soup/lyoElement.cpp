#include "lyoElement.hpp"

#include "lyoContainer.hpp"
#include "lyoFlatDocument.hpp"
#include "RenderTarget.hpp"

NAMESPACE_SOUP
{
	lyoDocument& lyoElement::getDocument() noexcept
	{
		lyoElement* elm = this;
		while (elm->parent != nullptr)
		{
			elm = static_cast<lyoElement*>(elm->parent);
		}
		return *reinterpret_cast<lyoDocument*>(this); // using reinterpret_cast because lyoDocument is not known in this compilation unit
	}

	bool lyoElement::matchesSelector(const std::string& selector) const noexcept
	{
		return tag_name == selector;
	}

	void lyoElement::querySelectorAll(std::vector<lyoElement*>& res, const std::string& selector)
	{
		if (matchesSelector(selector))
		{
			res.emplace_back(this);
		}
	}

	void lyoElement::propagateStyle()
	{
		style.propagateFromParent(parent->style);
	}

	void lyoElement::populateFlatDocument(lyoFlatDocument& fdoc)
	{
		fdoc.elms.emplace_back(this);
	}

	void lyoElement::updateFlatPos(unsigned int& x, unsigned int& y, unsigned int& wrap_y)
	{
		x += style.margin_left;
		if (style.display_block)
		{
			y += style.margin_top;
		}
	}

	void lyoElement::setFlatPos(unsigned int x, unsigned int y)
	{
		flat_x = x;
		flat_y = y;
	}

	void lyoElement::wrapLine(unsigned int& x, unsigned int& y, unsigned int& wrap_y)
	{
		if (x != parent->flat_x)
		{
			x = parent->flat_x;
			y = wrap_y + 3;
		}
	}

	void lyoElement::narrowFlatSize()
	{
	}

	void lyoElement::draw(RenderTarget& rt) const
	{
		if (style.background_color.has_value())
		{
			rt.drawRect(flat_x, flat_y, flat_width, flat_height, style.background_color.value());
		}
	}
}
