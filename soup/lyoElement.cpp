#include "lyoElement.hpp"

#include "lyoContainer.hpp"
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

	void lyoElement::propagateStyle()
	{
		style.propagateFromParent(parent->style);
	}

	void lyoElement::updateFlatPos()
	{
		if (parent != nullptr)
		{
			flat_x = parent->flat_x;
			flat_y = parent->flat_y;
		}

		flat_x += style.margin_left;
		if (!style.display_inline)
		{
			flat_y += style.margin_top;
		}

		if (parent != nullptr)
		{
			if ((flat_x + flat_width >= parent->flat_width || !style.display_inline)
				&& flat_x != parent->style.margin_left
				)
			{
				flat_x = parent->style.margin_left;
				flat_y += flat_height + 3;
			}

			parent->flat_x = flat_x;
			parent->flat_y = flat_y;
		}
	}

	void lyoElement::draw(RenderTarget& rt) const
	{
		if (style.background_color.has_value())
		{
			rt.drawRect(flat_x, flat_y, flat_width, flat_height, style.background_color.value());
		}
	}
}
