#pragma once

#include <cstdint>

namespace soup
{
	enum chess_piece : uint8_t
	{
		PIECE_NONE = 0,
		PIECE_PAWN = 10,
		PIECE_KNIGHT = 27,
		PIECE_BISHOP = 30,
		PIECE_ROOK = 50,
		PIECE_QUEEN = 90,
		PIECE_KING = 255,
	};
}
