#pragma once

#include "lyoTextElement.hpp"

NAMESPACE_SOUP
{
	struct lyoInputElement : public lyoTextElement
	{
		lyoInputElement(lyoContainer* parent);
	};
}
