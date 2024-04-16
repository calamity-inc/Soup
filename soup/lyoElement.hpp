#pragma once

#include "fwd.hpp"

#include <string>
#include <vector>

#include "lyoStyle.hpp"

#define LYO_DEBUG_POS		false
#define LYO_DEBUG_STYLE		false

NAMESPACE_SOUP
{
	struct lyoElement
	{
		lyoContainer* parent;

		unsigned int flat_x;
		unsigned int flat_y;
		unsigned int flat_width;
		unsigned int flat_height;

		std::string tag_name{};

		lyoStyle style;

		void(*on_click)(lyoElement&, lyoDocument&) = nullptr;

		lyoElement(lyoContainer* parent)
			: parent(parent)
		{
		}

		virtual ~lyoElement() = default;

		[[nodiscard]] lyoDocument& getDocument() noexcept;

		[[nodiscard]] bool matchesSelector(const std::string& selector) const noexcept;
		virtual void querySelectorAll(std::vector<lyoElement*>& res, const std::string& selector);

		virtual void propagateStyle();

		virtual void populateFlatDocument(lyoFlatDocument& fdoc);
		virtual void updateFlatValues(unsigned int& x, unsigned int& y, unsigned int& wrap_y);
		void setFlatPos(unsigned int x, unsigned int y);
		void wrapLine(unsigned int& x, unsigned int& y, unsigned int& wrap_y);

		virtual void draw(RenderTarget& rt) const;
	};
}
