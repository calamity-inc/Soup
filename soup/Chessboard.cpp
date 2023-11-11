#include "Chessboard.hpp"

#include <cstring> // memset

#include "rand.hpp"
#include "string.hpp"

namespace soup
{
	Chessboard::Chessboard()
	{
		setDefaultPieces();
	}

	ChessSquare& Chessboard::squareByReadingOrderIndex(uint8_t idx)
	{
		idx = (num_squares - idx - 1);
		return squares.at(
			((idx & 0b111) << 3)
			| (idx >> 3)
		);
	}

	ChessSquare& Chessboard::square(uint8_t file, uint8_t rank)
	{
		return squares.at((file << 3) + rank);
	}

	const ChessSquare& Chessboard::square(uint8_t file, uint8_t rank) const
	{
		return squares.at((file << 3) + rank);
	}

	ChessSquare& Chessboard::square(ChessCoordinate coord)
	{
		return square(coord.file, coord.rank);
	}

	const ChessSquare& Chessboard::square(ChessCoordinate coord) const
	{
		return square(coord.file, coord.rank);
	}

	void Chessboard::loadDefaultPosition() noexcept
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

	void Chessboard::setDefaultPieces() noexcept
	{
		fillRankWithPieces(0, false);
		fillRankWithPawns(1, false);
		fillRankWithPawns(6, true);
		fillRankWithPieces(7, true);
	}

	void Chessboard::resetRank(uint8_t rank)
	{
		for (uint8_t file = 0; file != 8; ++file)
		{
			square(file, rank).reset();
		}
	}

	void Chessboard::fillRankWithPawns(uint8_t rank, bool black)
	{
		for (uint8_t file = 0; file != 8; ++file)
		{
			square(file, rank) = ChessSquare{ PIECE_PAWN, black };
		}
	}

	void Chessboard::fillRankWithPieces(uint8_t rank, bool black)
	{
		square(0, rank) = ChessSquare{ PIECE_ROOK, black };
		square(1, rank) = ChessSquare{ PIECE_KNIGHT, black };
		square(2, rank) = ChessSquare{ PIECE_BISHOP, black };
		square(3, rank) = ChessSquare{ PIECE_QUEEN, black };
		square(4, rank) = ChessSquare{ PIECE_KING, black };
		square(5, rank) = ChessSquare{ PIECE_BISHOP, black };
		square(6, rank) = ChessSquare{ PIECE_KNIGHT, black };
		square(7, rank) = ChessSquare{ PIECE_ROOK, black };
	}

	void Chessboard::reset()
	{
		memset(&squares[0], 0, sizeof(squares));
	}

	bool Chessboard::loadFen(std::string fen)
	{
		string::limit<std::string>(fen, " ");
		/*auto fen_ranks = string::explode(fen, "/");
		if (fen_ranks.size() != 8)
		{
			return false;
		}*/
		reset();
		uint8_t i = 0;
		for (const auto& c : fen)
		{
			if (i == num_squares)
			{
				return false;
			}
			auto b = static_cast<uint8_t>(c);
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

	std::string Chessboard::toString(bool inverted) const noexcept
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

	const char* Chessboard::toString(ChessCoordinate coord, bool inverted) const noexcept
	{
		return toString(coord.file, coord.rank, inverted);
	}

	const char* Chessboard::toString(uint8_t file, uint8_t rank, bool inverted) const noexcept
	{
		auto piece_symbol = square(file, rank).toString(inverted);
		if (piece_symbol)
		{
			return piece_symbol;
		}
		return (const char*)((((rank + file) % 2) ^ !inverted) ? u8"■" : u8"□");
	}

	void Chessboard::playMove(ChessCoordinate from, ChessCoordinate to)
	{
		square(to) = square(from);
		square(from).reset();
		last_move = { from, to };
	}

	void Chessboard::playRandomMove(bool black)
	{
		std::vector<ChessCoordinate> my_pieces{};
		for (uint8_t file = 0; file != 8; ++file)
		{
			for (uint8_t rank = 0; rank != 8; ++rank)
			{
				ChessCoordinate coord(file, rank);
				if (!square(coord).isEmpty()
					&& square(coord).black == black
					)
				{
					my_pieces.emplace_back(coord);
				}
			}
		}
		if (my_pieces.empty())
		{
			return;
		}
		ChessCoordinate move_from;
		std::vector<ChessCoordinate> move_to_options{};
		ChessCoordinate move_to;
		while (true)
		{
			move_from = rand(my_pieces);
			move_to_options = square(move_from).getMoves(move_from);
			move_to = rand(move_to_options);

			// Shitty move validation
			if (square(move_to).isEmpty()
				|| square(move_to).black != black
				)
			{
				break;
			}
		}
		playMove(move_from, move_to);
	}
}
