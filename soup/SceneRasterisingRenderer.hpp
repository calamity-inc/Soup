#pragma once

#include "SceneRenderer.hpp"

namespace soup
{
	struct SceneRasterisingRenderer : public SceneRenderer
	{
		float z_near = 0.1f;
		float z_far = 1000.0f;

		void render(const Scene& s, RenderTarget& rt, float fov) const final;
	};
}
