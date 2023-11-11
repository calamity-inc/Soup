#include "ChessSquare.hpp"

#include "ChessCoordinate.hpp"

namespace soup
{
	const char* ChessSquare::toString(bool inverted) const noexcept
	{
		if (white() ^ inverted)
		{
			switch (piece)
			{
			case PIECE_NONE:
				break;

			case PIECE_PAWN:
				return (const char*)u8"♙";

			case PIECE_KNIGHT:
				return (const char*)u8"♘";

			case PIECE_BISHOP:
				return (const char*)u8"♗";

			case PIECE_ROOK:
				return (const char*)u8"♖";

			case PIECE_QUEEN:
				return (const char*)u8"♕";

			case PIECE_KING:
				return (const char*)u8"♔";
			}
		}
		else
		{
			switch (piece)
			{
			case PIECE_NONE:
				break;

			case PIECE_PAWN:
				return (const char*)u8"♟";

			case PIECE_KNIGHT:
				return (const char*)u8"♞";

			case PIECE_BISHOP:
				return (const char*)u8"♝";

			case PIECE_ROOK:
				return (const char*)u8"♜";

			case PIECE_QUEEN:
				return (const char*)u8"♛";

			case PIECE_KING:
				return (const char*)u8"♚";
			}
		}
		return nullptr;
	}

	std::vector<ChessCoordinate> ChessSquare::getMoves(ChessCoordinate coord)
	{
		return getMoves(coord.file, coord.rank);
	}

	std::vector<ChessCoordinate> ChessSquare::getMoves(uint8_t file, uint8_t rank)
	{
		std::vector<ChessCoordinate> res{};
		if (piece == PIECE_PAWN)
		{
			if (white())
			{
				addMoveIfInBounds(res, file, rank + 1);
				if (rank == 1)
				{
					addMoveIfInBounds(res, file, rank + 2);
				}
			}
			else
			{
				addMoveIfInBounds(res, file, rank - 1);
				if (rank == 6)
				{
					addMoveIfInBounds(res, file, rank - 2);
					// TODO: en croissant
				}
			}
		}
		else if (piece == PIECE_KNIGHT)
		{
			addMoveIfInBounds(res, file + 2, rank + 1);
			addMoveIfInBounds(res, file + 2, rank - 1);
			addMoveIfInBounds(res, file - 2, rank + 1);
			addMoveIfInBounds(res, file - 2, rank - 1);
			addMoveIfInBounds(res, file + 1, rank + 2);
			addMoveIfInBounds(res, file + 1, rank - 2);
			addMoveIfInBounds(res, file - 1, rank + 2);
			addMoveIfInBounds(res, file - 1, rank - 2);
		}
		else if (piece == PIECE_KING)
		{
			addMoveIfInBounds(res, file + 1, rank + 1);
			addMoveIfInBounds(res, file, rank + 1);
			addMoveIfInBounds(res, file - 1, rank + 1);
			addMoveIfInBounds(res, file + 1, rank);
			addMoveIfInBounds(res, file - 1, rank);
			addMoveIfInBounds(res, file + 1, rank - 1);
			addMoveIfInBounds(res, file, rank - 1);
			addMoveIfInBounds(res, file - 1, rank - 1);
		}
		else
		{
			if (piece == PIECE_ROOK || piece == PIECE_QUEEN)
			{
				for (uint8_t file_ = file; file_-- != 0; )
				{
					res.emplace_back(file_, rank);
				}
				for (uint8_t file_ = file; ++file_ != 8; )
				{
					res.emplace_back(file_, rank);
				}
				for (uint8_t rank_ = rank; rank_-- != 0; )
				{
					res.emplace_back(file, rank_);
				}
				for (uint8_t rank_ = rank; ++rank_ != 8; )
				{
					res.emplace_back(file, rank_);
				}
			}
			if (piece == PIECE_BISHOP || piece == PIECE_QUEEN)
			{
				for (uint8_t file_ = file, rank_ = rank; !(file_-- == 0 || rank_-- == 0); )
				{
					res.emplace_back(file_, rank_);
				}
				for (uint8_t file_ = file, rank_ = rank; !(++file_ == 8 || rank_-- == 0); )
				{
					res.emplace_back(file_, rank_);
				}
				for (uint8_t file_ = file, rank_ = rank; !(file_-- == 0 || ++rank_ == 8); )
				{
					res.emplace_back(file_, rank_);
				}
				for (uint8_t file_ = file, rank_ = rank; ++file_ != 8 && ++rank_ != 8; )
				{
					res.emplace_back(file_, rank_);
				}
			}
		}
		return res;
	}

	void ChessSquare::addMoveIfInBounds(std::vector<ChessCoordinate>& res, uint8_t file, uint8_t rank) noexcept
	{
		ChessCoordinate coord(file, rank);
		if (coord.isValid())
		{
			res.emplace_back(std::move(coord));
		}
	}
}
