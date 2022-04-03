#pragma once

#include <vector>

#include "fwd.hpp"
#include "chess_piece.hpp"

namespace soup
{
	class chess_square
	{
	public:
		chess_piece piece;
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

		[[nodiscard]] std::vector<chess_coordinate> getMoves(chess_coordinate coord);
		[[nodiscard]] std::vector<chess_coordinate> getMoves(uint8_t file, uint8_t rank);
	protected:
		static void addMoveIfInBounds(std::vector<chess_coordinate>& res, uint8_t file, uint8_t rank) noexcept;
	};
}
