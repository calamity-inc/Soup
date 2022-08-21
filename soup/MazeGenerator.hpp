#pragma once

#include "fwd.hpp"

namespace soup
{
	struct MazeGenerator
	{
		void generate(algRng& rng)
		{
			while (!isFinished())
			{
				tick(rng);
			}
		}

		[[nodiscard]] virtual bool isFinished() = 0;
		virtual void tick(algRng& rng) = 0;
	};
}
