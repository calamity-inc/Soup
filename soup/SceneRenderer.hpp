#pragma once

#include "fwd.hpp"

NAMESPACE_SOUP
{
	struct SceneRenderer
	{
		virtual void render(const Scene& s, RenderTarget& rt, float fov) const = 0;
	};
}
