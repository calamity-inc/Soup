#pragma once

#include <cstdint>

#include "base.hpp"

NAMESPACE_SOUP
{
	enum ChessPiece : uint8_t
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
