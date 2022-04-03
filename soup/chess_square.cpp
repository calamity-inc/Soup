#include "chess_square.hpp"

#include "chess_coordinate.hpp"

namespace soup
{
	const char* chess_square::toString(bool inverted) const noexcept
	{
		if (white() ^ inverted)
		{
			switch (piece)
			{
			case PIECE_NONE:
				break;

			case PIECE_PAWN:
				return "♙";

			case PIECE_KNIGHT:
				return "♘";

			case PIECE_BISHOP:
				return "♗";

			case PIECE_ROOK:
				return "♖";

			case PIECE_QUEEN:
				return "♕";

			case PIECE_KING:
				return "♔";
			}
		}
		else
		{
			switch (piece)
			{
			case PIECE_NONE:
				break;

			case PIECE_PAWN:
				return "♟";

			case PIECE_KNIGHT:
				return "♞";

			case PIECE_BISHOP:
				return "♝";

			case PIECE_ROOK:
				return "♜";

			case PIECE_QUEEN:
				return "♛";

			case PIECE_KING:
				return "♚";
			}
		}
		return nullptr;
	}

	std::vector<chess_coordinate> chess_square::getMoves(chess_coordinate coord)
	{
		return getMoves(coord.file, coord.rank);
	}

	std::vector<chess_coordinate> chess_square::getMoves(uint8_t file, uint8_t rank)
	{
		std::vector<chess_coordinate> res{};
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

	void chess_square::addMoveIfInBounds(std::vector<chess_coordinate>& res, uint8_t file, uint8_t rank) noexcept
	{
		chess_coordinate coord(file, rank);
		if (coord.isValid())
		{
			res.emplace_back(std::move(coord));
		}
	}
}
