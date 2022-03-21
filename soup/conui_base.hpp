#pragma once

#include "fwd.hpp"

namespace soup
{
	struct conui_base
	{
		conui_div* parent;
		unsigned int x;
		unsigned int y;
		unsigned int width;
		unsigned int height;

		conui_base(conui_div* parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
			: parent(parent), x(x), y(y), width(width), height(height)
		{
		}

		virtual ~conui_base() = default;
		virtual void draw() const = 0;
		virtual void onClick(mouse_button b, unsigned int x, unsigned int y);

		[[nodiscard]] conui_app* getApp() noexcept;
		[[nodiscard]] const conui_app* getApp() const noexcept;
	};
}
