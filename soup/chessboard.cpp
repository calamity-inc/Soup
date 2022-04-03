#include "chessboard.hpp"

#include "chess_coordinate.hpp"
#include "string.hpp"

namespace soup
{
	chessboard::chessboard()
	{
		setDefaultPieces();
	}

	chess_square& chessboard::squareByReadingOrderIndex(uint8_t idx)
	{
		idx = (num_squares - idx - 1);
		return squares.at(
			((idx & 0b111) << 3)
			| (idx >> 3)
		);
	}

	chess_square& chessboard::square(uint8_t file, uint8_t rank)
	{
		return squares.at((file << 3) + rank);
	}

	const chess_square& chessboard::square(uint8_t file, uint8_t rank) const
	{
		return squares.at((file << 3) + rank);
	}

	chess_square& soup::chessboard::square(chess_coordinate coord)
	{
		return square(coord.file, coord.rank);
	}

	const chess_square& chessboard::square(chess_coordinate coord) const
	{
		return square(coord.file, coord.rank);
	}

	void chessboard::loadDefaultPosition() noexcept
	{
		fillRankWithPieces(0, false);
		fillRankWithPawns(1, false);
		resetRank(2);
		resetRank(3);
		resetRank(4);
		resetRank(5);
		fillRankWithPawns(6, true);
		fillRankWithPieces(7, true);
	}

	void chessboard::setDefaultPieces() noexcept
	{
		fillRankWithPieces(0, false);
		fillRankWithPawns(1, false);
		fillRankWithPawns(6, true);
		fillRankWithPieces(7, true);
	}

	void chessboard::resetRank(uint8_t rank)
	{
		for (uint8_t file = 0; file != 8; ++file)
		{
			square(file, rank).reset();
		}
	}

	void chessboard::fillRankWithPawns(uint8_t rank, bool black)
	{
		for (uint8_t file = 0; file != 8; ++file)
		{
			square(file, rank) = chess_square{ PIECE_PAWN, black };
		}
	}

	void chessboard::fillRankWithPieces(uint8_t rank, bool black)
	{
		square(0, rank) = chess_square{ PIECE_ROOK, black };
		square(1, rank) = chess_square{ PIECE_KNIGHT, black };
		square(2, rank) = chess_square{ PIECE_BISHOP, black };
		square(3, rank) = chess_square{ PIECE_QUEEN, black };
		square(4, rank) = chess_square{ PIECE_KING, black };
		square(5, rank) = chess_square{ PIECE_BISHOP, black };
		square(6, rank) = chess_square{ PIECE_KNIGHT, black };
		square(7, rank) = chess_square{ PIECE_ROOK, black };
	}

	void chessboard::reset()
	{
		memset(&squares[0], 0, sizeof(squares));
	}

	bool chessboard::loadFen(std::string fen)
	{
		string::limit<std::string>(fen, " ");
		/*auto fen_ranks = string::explode(fen, "/");
		if (fen_ranks.size() != 8)
		{
			return false;
		}*/
		reset();
		size_t i = 0;
		for (const auto& c : fen)
		{
			if (i == num_squares)
			{
				return false;
			}
			auto b = (uint8_t)c;
			const bool black = ((b >> 5) & 1);
			switch (c)
			{
			case 'p':
			case 'P':
				squareByReadingOrderIndex(i++) = { PIECE_PAWN, black };
				break;

			case 'n':
			case 'N':
				squareByReadingOrderIndex(i++) = { PIECE_KNIGHT, black };
				break;

			case 'b':
			case 'B':
				squareByReadingOrderIndex(i++) = { PIECE_BISHOP, black };
				break;

			case 'r':
			case 'R':
				squareByReadingOrderIndex(i++) = { PIECE_ROOK, black };
				break;

			case 'q':
			case 'Q':
				squareByReadingOrderIndex(i++) = { PIECE_QUEEN, black };
				break;

			case 'k':
			case 'K':
				squareByReadingOrderIndex(i++) = { PIECE_KING, black };
				break;

			case '1': i += 1; break;
			case '2': i += 2; break;
			case '3': i += 3; break;
			case '4': i += 4; break;
			case '5': i += 5; break;
			case '6': i += 6; break;
			case '7': i += 7; break;
			case '8': i += 8; break;
			}
		}
		return true;
	}

	std::string chessboard::toString(bool inverted) const noexcept
	{
		std::string str{};
		for (uint8_t rank = 8; rank-- != 0; )
		{
			for (uint8_t file = 0; file != 8; ++file)
			{
				str.append(toString(file, rank, inverted));
			}
			str.push_back('\n');
		}
		return str;
	}

	const char* chessboard::toString(chess_coordinate coord, bool inverted) const noexcept
	{
		return toString(coord.file, coord.rank, inverted);
	}

	const char* chessboard::toString(uint8_t file, uint8_t rank, bool inverted) const noexcept
	{
		auto piece_symbol = square(file, rank).toString(inverted);
		if (piece_symbol)
		{
			return piece_symbol;
		}
		return ((((rank + file) % 2) ^ !inverted) ? "■" : "□");
	}
}
