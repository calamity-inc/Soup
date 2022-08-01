#pragma once

#include "SceneRenderer.hpp"

namespace soup
{
	// Adapted from https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_olcEngine3D_Part3.cpp
	// with thanks to One Lone Coder for explaining rasterisation in reasonably understandable terms

	struct SceneRasterisingRenderer : public SceneRenderer
	{
		float z_near = 0.00001f;
		float z_far = 1000.0f;
		bool no_sky_draw = false;

		void render(const Scene& s, RenderTarget& rt, float fov) const final;
	};
}
