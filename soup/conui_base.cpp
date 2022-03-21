#include "conui_base.hpp"

namespace soup
{
	void conui_base::onClick(mouse_button b, unsigned int x, unsigned int y)
	{
		if (parent != nullptr)
		{
			reinterpret_cast<conui_base*>(parent)->onClick(b, x, y);
		}
	}

	conui_app* conui_base::getApp() noexcept
	{
		if (parent == nullptr)
		{
			return reinterpret_cast<conui_app*>(this);
		}
		return reinterpret_cast<conui_base*>(parent)->getApp();
	}

	const conui_app* conui_base::getApp() const noexcept
	{
		if (parent == nullptr)
		{
			return reinterpret_cast<const conui_app*>(this);
		}
		return reinterpret_cast<const conui_base*>(parent)->getApp();
	}
}
