#pragma once

#include "lyoElement.hpp"

#include <string>
#include <vector>

#include "UniquePtr.hpp"

namespace soup
{
	struct lyoContainer : public lyoElement
	{
		std::vector<UniquePtr<lyoElement>> children;

		using lyoElement::lyoElement;

		lyoTextElement* addText(const std::string& text);
		lyoTextElement* addText(std::u32string text);

		[[nodiscard]] lyoElement* querySelector(const std::string& selector);

		void flattenElement(lyoFlatDocument& flat) final;
	};
}
