#pragma once

#include "fwd.hpp"

namespace soup
{
	struct MazeGenerator
	{
		void generate(RandomDevice& rd)
		{
			while (!isFinished())
			{
				tick(rd);
			}
		}

		[[nodiscard]] virtual bool isFinished() = 0;
		virtual void tick(RandomDevice& rd) = 0;
	};
}
