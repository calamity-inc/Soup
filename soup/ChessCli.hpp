#pragma once

#include <utility> // pair

#include "Chessboard.hpp"
#include "ChessCoordinate.hpp"

NAMESPACE_SOUP
{
	struct ChessCli
	{
		bool dark_theme = true;
		Chessboard board{};
		ChessCoordinate selection{};

		[[nodiscard]] static ChessCoordinate console2chess(unsigned int x, unsigned int y)
		{
			return ChessCoordinate(x, 7 - y);
		}

		[[nodiscard]] static std::pair<unsigned int, unsigned int> chess2console(ChessCoordinate coord)
		{
			return { coord.file, 7 - coord.rank };
		}

		void run();
	};
}
