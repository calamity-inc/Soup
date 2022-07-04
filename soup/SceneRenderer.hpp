#pragma once

#include "fwd.hpp"

namespace soup
{
	struct SceneRenderer
	{
		virtual void render(const Scene& s, RenderTarget& rt, float fov) const = 0;
	};
}
