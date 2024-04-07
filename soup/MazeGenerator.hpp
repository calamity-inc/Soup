#pragma once

#include "fwd.hpp"

NAMESPACE_SOUP
{
	struct MazeGenerator
	{
		void generate(RngInterface& rng)
		{
			while (!isFinished())
			{
				tick(rng);
			}
		}

		[[nodiscard]] virtual bool isFinished() = 0;
		virtual void tick(RngInterface& rng) = 0;
	};
}
