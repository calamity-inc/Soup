#include "ConuiBase.hpp"

namespace soup
{
	void ConuiBase::onClick(MouseButton b, unsigned int x, unsigned int y)
	{
		if (parent != nullptr)
		{
			reinterpret_cast<ConuiBase*>(parent)->onClick(b, x, y);
		}
	}

	ConuiApp* ConuiBase::getApp() noexcept
	{
		if (parent == nullptr)
		{
			return reinterpret_cast<ConuiApp*>(this);
		}
		return reinterpret_cast<ConuiBase*>(parent)->getApp();
	}

	const ConuiApp* ConuiBase::getApp() const noexcept
	{
		if (parent == nullptr)
		{
			return reinterpret_cast<const ConuiApp*>(this);
		}
		return reinterpret_cast<const ConuiBase*>(parent)->getApp();
	}
}
