#pragma once

#include <vector>

#include "fwd.hpp"
#include "ChessPiece.hpp"

namespace soup
{
	class ChessSquare
	{
	public:
		ChessPiece piece;
		bool black;

		[[nodiscard]] constexpr bool isEmpty() const noexcept
		{
			return piece != PIECE_NONE;
		}

		void reset() noexcept
		{
			piece = PIECE_NONE;
		}

		[[nodiscard]] constexpr bool white() const noexcept
		{
			return !black;
		}

		const char* toString(bool inverted = false) const noexcept;

		[[nodiscard]] std::vector<ChessCoordinate> getMoves(ChessCoordinate coord);
		[[nodiscard]] std::vector<ChessCoordinate> getMoves(uint8_t file, uint8_t rank);
	protected:
		static void addMoveIfInBounds(std::vector<ChessCoordinate>& res, uint8_t file, uint8_t rank) noexcept;
	};
}
