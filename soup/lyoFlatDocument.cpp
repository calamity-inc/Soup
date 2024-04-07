#include "lyoFlatDocument.hpp"

#include "lyoElement.hpp"

NAMESPACE_SOUP
{
	void lyoFlatDocument::draw(RenderTarget& rt) const
	{
		for (const auto& e : elms)
		{
			e->draw(rt);
		}
	}

	lyoElement* lyoFlatDocument::getElementAtPos(unsigned int x, unsigned int y) const
	{
		auto i = elms.rbegin();
		for (; i != elms.rend(); ++i)
		{
			if (x >= (*i)->flat_x
				&& y >= (*i)->flat_y
				&& x < (*i)->flat_x + (*i)->flat_width
				&& y < (*i)->flat_y + (*i)->flat_height
				)
			{
				return *i;
			}
		}
		return nullptr;
	}
}
