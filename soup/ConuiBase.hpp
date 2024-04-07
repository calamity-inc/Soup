#pragma once

#include "fwd.hpp"

NAMESPACE_SOUP
{
	struct ConuiBase
	{
		ConuiDiv* parent;
		unsigned int x;
		unsigned int y;
		unsigned int width;
		unsigned int height;

		ConuiBase(ConuiDiv* parent, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
			: parent(parent), x(x), y(y), width(width), height(height)
		{
		}

		virtual ~ConuiBase() = default;
		virtual void draw() const = 0;
		virtual void onClick(MouseButton b, unsigned int x, unsigned int y);

		[[nodiscard]] ConuiApp* getApp() noexcept;
		[[nodiscard]] const ConuiApp* getApp() const noexcept;
	};
}
