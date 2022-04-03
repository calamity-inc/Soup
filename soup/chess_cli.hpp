#pragma once

#include <utility> // pair

#include "chessboard.hpp"
#include "chess_coordinate.hpp"

namespace soup
{
	struct chess_cli
	{
		bool dark_theme = true;
		chessboard board{};
		chess_coordinate selection{};

		[[nodiscard]] static chess_coordinate console2chess(unsigned int x, unsigned int y)
		{
			return chess_coordinate(x, 7 - y);
		}

		[[nodiscard]] static std::pair<unsigned int, unsigned int> chess2console(chess_coordinate coord)
		{
			return { coord.file, 7 - coord.rank };
		}

		void run();
	};
}
