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
		[[nodiscard]] std::vector<lyoElement*> querySelectorAll(const std::string& selector);
		void querySelectorAll(std::vector<lyoElement*>& res, const std::string& selector) final;

		void propagateStyle() override;
		void propagateStyleToChildren() const;

		void populateFlatDocument(lyoFlatDocument& fdoc) final;
		void updateFlatSize() final;
		void updateFlatPos(unsigned int& x, unsigned int& y, unsigned int& wrap_y) final;
		void narrowFlatSize() final;
	};
}
