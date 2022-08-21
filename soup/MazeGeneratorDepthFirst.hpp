#pragma once

#include "MazeGenerator.hpp"

#include <stack>
#include <vector>

namespace soup
{
	struct MazeGeneratorDepthFirst : public MazeGenerator
	{
		enum Cell : uint8_t
		{
			VISITED = 1 << 0,
			CON_TOP = 1 << 1,
			CON_LEFT = 1 << 2,
			CON_RIGHT = 1 << 3,
			CON_DOWN = 1 << 4,
		};

		const size_t width;
		const size_t height;
		std::vector<uint8_t> cells{};
		std::stack<size_t> backtrace{};

		MazeGeneratorDepthFirst(size_t width, size_t height);

		[[nodiscard]] bool isFinished() final;
		void tick(algRng& rng) final;

		std::pair<size_t, size_t> getRenderSize() const;
		void render(RenderTarget& rt) const;
	};
}
