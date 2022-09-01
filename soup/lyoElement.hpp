#pragma once

#include "fwd.hpp"
#include "TreeNode.hpp"

#include <string>

#include "lyoStyle.hpp"

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

		lyoStyle style;

		void(*on_click)(lyoElement&, lyoDocument&) = nullptr;

		lyoElement(lyoContainer* parent)
			: parent(parent)
		{
		}

		[[nodiscard]] lyoDocument& getDocument() noexcept;

		[[nodiscard]] bool matchesSelector(const std::string& selector) const noexcept;

		virtual void populateFlatDocument(lyoFlatDocument& fdoc) = 0;
		virtual void updateFlatPos() = 0;
		virtual void updateFlatSize() = 0;

		virtual void draw(RenderTarget& rt) const
		{
		}
	};
}
