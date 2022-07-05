#pragma once

#include <array>
#include <string>

#include "fwd.hpp"
#include "ChessCoordinate.hpp"
#include "ChessSquare.hpp"

namespace soup
{
	class Chessboard
	{
	public:
		struct Move
		{
			ChessCoordinate from;
			ChessCoordinate to;
		};

		static constexpr auto num_squares = (8 * 8);

		std::array<ChessSquare, num_squares> squares{};
		Move last_move{};

		Chessboard();

		ChessSquare& squareByReadingOrderIndex(uint8_t idx);

		ChessSquare& square(uint8_t file, uint8_t rank);

		const ChessSquare& square(uint8_t file, uint8_t rank) const;

		ChessSquare& square(ChessCoordinate coord);
		const ChessSquare& square(ChessCoordinate coord) const;

		void loadDefaultPosition() noexcept;
	protected:
		void setDefaultPieces() noexcept;
		void resetRank(uint8_t rank);
		void fillRankWithPawns(uint8_t rank, bool black);
		void fillRankWithPieces(uint8_t rank, bool black);

	public:
		void reset();

		bool loadFen(std::string fen);

		[[nodiscard]] std::string toString(bool inverted = false) const noexcept;
		[[nodiscard]] const char* toString(ChessCoordinate coord, bool inverted = false) const noexcept;
		[[nodiscard]] const char* toString(uint8_t file, uint8_t rank, bool inverted = false) const noexcept;

		void playMove(ChessCoordinate from, ChessCoordinate to);

		void playRandomMove(bool black = true);
	};
}
