#include "lyoElement.hpp"

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
}
