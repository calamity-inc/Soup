#pragma once

#include <cstdint>
#include <string>

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
	/* To print to console:
	console.init(false);
	Canvas c(15 * 9, 15 * 9);
	RenderTargetCanvas rt(c);
	s.draw(rt);
	console << c.toStringDownsampledDoublewidth(true);
	*/
	/* To watch it slowly solve a Sudoku:
	console.init(false);
	Canvas c(15 * 9, 15 * 9);
	RenderTargetCanvas rt(c);
	do
	{
		s.draw(rt);
		console.clearScreen();
		console.setCursorPos(0, 0);
		console << c.toStringDownsampledDoublewidth(true);

		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		s.eliminateImpossibleCandiates();

		s.draw(rt);
		console.clearScreen();
		console.setCursorPos(0, 0);
		console << c.toStringDownsampledDoublewidth(true);

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	} while (s.step());
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

		[[nodiscard]] static value_t maskToValue(mask_t mask) noexcept;

		struct Cell
		{
			uint32_t value_bf : 9;
			uint32_t value_was_given : 1;
			uint32_t candidates_bf : 9;

			Cell()
				: value_bf(0), value_was_given(false), candidates_bf(0b111'111'111)
			{
			}

			void reset() noexcept
			{
				value_bf = 0;
				value_was_given = false;
				candidates_bf = 0b111'111'111;
			}

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

			void draw(RenderTarget& rt, bool no_candidates, int x, int y) const;
		};

		Cell cells[9 * 9]{};

		Sudoku() = default;

		Sudoku(value_t values[9 * 9])
		{
			for (index_t i = 0; i != 9 * 9; ++i)
			{
				if (values[i] != 0)
				{
					cells[i].setGiven(values[i]);
				}
			}
		}

		void reset() noexcept;

		void setGivenFromString(const char* str) noexcept;
		void setGivenFromBinary(const std::string& str);

		[[nodiscard]] static index_t getCellIndex(index_t x, index_t y) noexcept
		{
			//SOUP_ASSERT(x < 9 && y < 9);
			return x + (y * 9);
		}

		[[nodiscard]] Cell& getCell(index_t x, index_t y) noexcept
		{
			return cells[getCellIndex(x, y)];
		}

		[[nodiscard]] const Cell& getCell(index_t x, index_t y) const noexcept
		{
			return cells[getCellIndex(x, y)];
		}

		[[nodiscard]] static constexpr char getRowName(index_t y) noexcept
		{
			return 'A' + y;
		}

		[[nodiscard]] static constexpr char getColumnName(index_t x) noexcept
		{
			return '1' + x;
		}
		
		[[nodiscard]] static std::string getCellName(index_t x, index_t y)
		{
			std::string str(1, getRowName(y));
			str.push_back(getColumnName(x));
			return str;
		}

		[[nodiscard]] count_t getNumValues() const noexcept;

		[[nodiscard]] mask_t getValuesInBox(index_t i) const noexcept;
		[[nodiscard]] mask_t getValuesInBox(index_t bx, index_t by) const noexcept;
		[[nodiscard]] mask_t getValuesInRow(index_t y) const noexcept;
		[[nodiscard]] mask_t getValuesInColumn(index_t x) const noexcept;

		[[nodiscard]] mask_t getCandidatesInBox(mask_t value_bf, index_t i) const noexcept;
		[[nodiscard]] mask_t getCandidatesInBox(mask_t value_bf, index_t bx, index_t by) const noexcept;
		[[nodiscard]] mask_t getCandidatesInRow(mask_t value_bf, index_t y) const noexcept;
		[[nodiscard]] mask_t getCandidatesInColumn(mask_t value_bf, index_t x) const noexcept;

		[[nodiscard]] count_t getNumCandidatesInBox(mask_t value_bf, index_t bx, index_t by) const noexcept;
		[[nodiscard]] count_t getNumCandidatesInRow(mask_t value_bf, index_t y) const noexcept;
		[[nodiscard]] count_t getNumCandidatesInColumn(mask_t value_bf, index_t x) const noexcept;

		void eliminateImpossibleCandiates() noexcept;
		bool eliminateCandidate(mask_t value_bf, index_t x, index_t y) noexcept;
		bool narrowCandidatesInRowToBox(mask_t value_bf, index_t y, index_t pin_bx, std::string* explanation);
		bool narrowCandidatesInColumnToBox(mask_t value_bf, index_t x, index_t pin_by, std::string* explanation);
		bool narrowCandidatesInBoxToRow(mask_t value_bf, index_t bx, index_t by, index_t pin_y, std::string* explanation);
		bool narrowCandidatesInBoxToColumn(mask_t value_bf, index_t bx, index_t by, index_t pin_x, std::string* explanation);
		bool eliminateCandidatesInRow(mask_t value_bf, index_t y, index_t exclude_x1, index_t exclude_x2) noexcept;
		bool eliminateCandidatesInColumn(mask_t value_bf, index_t x, index_t exclude_y1, index_t exclude_y2) noexcept;

		[[nodiscard]] bool isSolvable() const noexcept; // Returns true if all missing digits have candidates.

		enum Strategies : uint8_t
		{
			NAKED_SINGLE = (1 << 0),
			HIDDEN_SINGLE = (1 << 1),
			LOCKED_CANDIDATES = (1 << 2),
			HIDDEN_PAIR = (1 << 3),
			X_WING = (1 << 4),
			CONTRADICTION_IF_CANDIDATE_REMOVED = (1 << 5),

			TRIVIAL = NAKED_SINGLE | HIDDEN_SINGLE,
			EASY = TRIVIAL | LOCKED_CANDIDATES | HIDDEN_PAIR,
			ALL = 0xff,
		};

		bool step(uint8_t strategies = ALL, std::string* explanation = nullptr);
		bool stepNakedSingle(std::string* explanation = nullptr); // Fill in a digit with only 1 candidate if possible.
		bool stepHiddenSingle(std::string* explanation = nullptr); // Fill in a digit that can only be in that place within a house if possible.
		bool stepLockedCandidates(std::string* explanation = nullptr); // Aka. pointing pairs/triples
		bool stepHiddenPair(std::string* explanation = nullptr);
		bool stepXWing(std::string* explanation = nullptr);
		bool stepContradictionIfCandidateRemoved(std::string* explanation = nullptr); // No idea what to call this. Similar idea to "Simple Colouring" and "X-Cycles".

		bool solveCell(std::string* explanation = nullptr);

		// Note: Might not actually be able to solve the puzzle. No bifurcation will be attempted.
		void solve(uint8_t strategies = ALL)
		{
			do
			{
				eliminateImpossibleCandiates();
			} while (step(strategies));
		}

		[[nodiscard]] bool canSolve(uint8_t strategies = ALL) const;

		void draw(RenderTarget& rt, bool no_candidates = false) const; // Requires a (15 * 9) by (15 * 9) pixel area.

		void fill(); // Fills the grid with random, valid digits until every cell is occupied.
		void removeRandomDigit();

		[[nodiscard]] std::string toString() const;
		[[nodiscard]] std::string toBinary() const;
		[[nodiscard]] std::string toBinaryV0() const; // very simple encoding, always takes up 41 bytes.
		[[nodiscard]] std::string toBinaryV1() const; // optimised for sparse grids, takes up 10,5 bytes plus 0,5 bytes per populated cell.
		[[nodiscard]] std::string toBinaryV2() const; // optimised for full grids, encoding them in only 25 bytes, but can encode other grids in 41 bytes or less.
	};
}
