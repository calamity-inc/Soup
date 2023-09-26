#pragma once

#include <cstdint>

#include "fwd.hpp"

namespace soup
{
	/* To load in a grid:
	constexpr auto _ = 0;
	uint8_t grid[] = {
		_,_,_,_,_,_,_,_,_,
		_,_,_,_,_,_,_,_,_,
		_,_,_,_,_,_,_,_,_,
		_,_,_,_,_,_,_,_,_,
		_,_,_,_,_,_,_,_,_,
		_,_,_,_,_,_,_,_,_,
		_,_,_,_,_,_,_,_,_,
		_,_,_,_,_,_,_,_,_,
		_,_,_,_,_,_,_,_,_,
	};
	Sudoku s(grid);
	*/
	struct Sudoku
	{
		using index_t = uint8_t;
		using value_t = uint8_t;
		using mask_t = uint16_t;
		using count_t = uint8_t;

		[[nodiscard]] static constexpr mask_t valueToMask(value_t value) noexcept
		{
			return (1 << (value - 1));
		}

		struct Field
		{
			uint32_t value_bf : 9 = 0;
			uint32_t value_was_given : 1 = false;
			uint32_t candidates_bf : 9 = 0b111'111'111;

			void setGiven(value_t value) noexcept
			{
				this->value_bf = valueToMask(value);
				this->value_was_given = true;
			}

			[[nodiscard]] value_t getValue() const noexcept;

			[[nodiscard]] count_t getNumCandidates() const noexcept;

			[[nodiscard]] bool isCandidateMask(mask_t mask) const noexcept
			{
				return candidates_bf & mask;
			}

			void draw(RenderTarget& rt, int x, int y) const;
		};

		Field fields[9 * 9]{};

		Sudoku(value_t values[9 * 9])
		{
			for (index_t i = 0; i != 9 * 9; ++i)
			{
				if (values[i] != 0)
				{
					fields[i].setGiven(values[i]);
				}
			}
		}

		[[nodiscard]] Field& getField(index_t x, index_t y) noexcept
		{
			return fields[x + (y * 9)];
		}

		[[nodiscard]] const Field& getField(index_t x, index_t y) const noexcept
		{
			return fields[x + (y * 9)];
		}

		[[nodiscard]] mask_t getValuesInBox(index_t i) const noexcept;
		[[nodiscard]] mask_t getValuesInBox(index_t bx, index_t by) const noexcept;
		[[nodiscard]] mask_t getValuesInRow(index_t y) const noexcept;
		[[nodiscard]] mask_t getValuesInColumn(index_t x) const noexcept;

		[[nodiscard]] mask_t getCandidatesInBox(mask_t value_bf, index_t bx, index_t by) const noexcept;
		[[nodiscard]] mask_t getCandidatesInRow(mask_t value_bf, index_t y) const noexcept;
		[[nodiscard]] mask_t getCandidatesInColumn(mask_t value_bf, index_t x) const noexcept;

		[[nodiscard]] count_t getNumCandidatesInBox(mask_t value_bf, index_t bx, index_t by) const noexcept;
		[[nodiscard]] count_t getNumCandidatesInRow(mask_t value_bf, index_t y) const noexcept;
		[[nodiscard]] count_t getNumCandidatesInColumn(mask_t value_bf, index_t x) const noexcept;

		void eliminateImpossibleCandiates() noexcept;
		bool eliminateCandidate(mask_t value_bf, index_t x, index_t y) noexcept;
		bool narrowCandidatesInRowToBox(mask_t value_bf, index_t y, index_t pin_bx) noexcept;
		bool narrowCandidatesInColumnToBox(mask_t value_bf, index_t x, index_t pin_by) noexcept;
		bool narrowCandidatesInBoxToRow(mask_t value_bf, index_t bx, index_t by, index_t pin_y) noexcept;
		bool narrowCandidatesInBoxToColumn(mask_t value_bf, index_t bx, index_t by, index_t pin_x) noexcept;
		bool eliminateCandidatesInRow(mask_t value_bf, index_t y, index_t exclude_x1, index_t exclude_x2) noexcept;
		bool eliminateCandidatesInColumn(mask_t value_bf, index_t x, index_t exclude_y1, index_t exclude_y2) noexcept;

		bool stepAny() noexcept;
		bool stepNakedSingle() noexcept; // Fill in a digit with only 1 candidate if possible.
		bool stepHiddenSingle() noexcept; // Fill in a digit that can only be in that place within a house if possible.
		bool stepLockedCandidates() noexcept;
		bool stepXWing() noexcept;

		// Note: Might not actually be able to solve the puzzle. No bifurcation will be attempted.
		void solve() noexcept
		{
			do
			{
				eliminateImpossibleCandiates();
			} while (stepAny());
		}

		void draw(RenderTarget& rt) const; // Requires a (15 * 9) by (15 * 9) pixel area.
	};
}