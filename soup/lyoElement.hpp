#pragma once

#include "fwd.hpp"
#include "TreeNode.hpp"

#include <string>

namespace soup
{
	struct lyoElement : public TreeNode
	{
		lyoContainer* parent;

		unsigned int flat_x;
		unsigned int flat_y;
		unsigned int flat_width;
		unsigned int flat_height;

		std::string tag_name{};

		bool display_inline = false;

		uint8_t margin_left = 0;
		uint8_t margin_right = 0;
		// margin-top & margin-bottom seemingly ignored with `display: inline`
		uint8_t margin_top = 0;
		uint8_t margin_bottom = 0;

		void(*on_click)(lyoElement&, lyoDocument&) = nullptr;

		lyoElement(lyoContainer* parent)
			: parent(parent)
		{
		}

		[[nodiscard]] lyoDocument& getDocument() noexcept;

		void setMargin(uint8_t margin)
		{
			margin_left = margin;
			margin_right = margin;
			margin_top = margin;
			margin_bottom = margin;
		}

		virtual void flattenElement(lyoFlatDocument& flat) = 0;

		virtual void draw(RenderTarget& rt) const
		{
		}
	};
}
