#include "lyoElement.hpp"

#include "RenderTarget.hpp"

namespace soup
{
	lyoDocument& lyoElement::getDocument() noexcept
	{
		lyoElement* elm = this;
		while (elm->parent != nullptr)
		{
			elm = reinterpret_cast<lyoElement*>(elm->parent); // We don't know that lyoContainer extends lyoElement
		}
		return *reinterpret_cast<lyoDocument*>(this);
	}

	bool lyoElement::matchesSelector(const std::string& selector) const noexcept
	{
		return tag_name == selector;
	}

	void lyoElement::draw(RenderTarget& rt) const
	{
		if (style.background_color.has_value())
		{
			rt.drawRect(flat_x, flat_y, flat_width, flat_height, style.background_color.value());
		}
	}
}
