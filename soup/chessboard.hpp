#pragma once

#include <array>
#include <string>

#include "fwd.hpp"
#include "chess_square.hpp"

namespace soup
{
	class chessboard
	{
	public:
		static constexpr auto num_squares = (8 * 8);

		std::array<chess_square, num_squares> squares{};

		chessboard();

		chess_square& squareByReadingOrderIndex(uint8_t idx);

		chess_square& square(uint8_t file, uint8_t rank);

		const chess_square& square(uint8_t file, uint8_t rank) const;

		chess_square& square(chess_coordinate coord);
		const chess_square& square(chess_coordinate coord) const;

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
		[[nodiscard]] const char* toString(chess_coordinate coord, bool inverted = false) const noexcept;
		[[nodiscard]] const char* toString(uint8_t file, uint8_t rank, bool inverted = false) const noexcept;
	};
}
