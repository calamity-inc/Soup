#include "Sudoku.hpp"

#include "BigBitset.hpp"
#include "BitReader.hpp"
#include "bitutil.hpp"
#include "BitWriter.hpp"
#include "format.hpp"
#include "rand.hpp"
#include "RasterFont.hpp"
#include "RenderTarget.hpp"
#include "Rgb.hpp"
#include "StringRefReader.hpp"
#include "StringWriter.hpp"

NAMESPACE_SOUP
{
	using index_t = Sudoku::index_t;
	using value_t = Sudoku::value_t;
	using mask_t = Sudoku::mask_t;
	using count_t = Sudoku::count_t;

	value_t Sudoku::maskToValue(mask_t mask) noexcept
	{
		return bitutil::getLeastSignificantSetBit(mask) + 1;
	}

	value_t Sudoku::Cell::getValue() const noexcept
	{
		SOUP_IF_LIKELY (value_bf != 0)
		{
			return maskToValue(value_bf);
		}
		return 0;
	}

	count_t Sudoku::Cell::getNumCandidates() const noexcept
	{
		return bitutil::getNumSetBits(candidates_bf);
	}

	void Sudoku::Cell::draw(RenderTarget& rt, bool no_candidates, int x, int y) const
	{
		rt.drawRect(x, y, 15, 15, Rgb::BLACK);
		if (value_bf != 0)
		{
			rt.drawText(x + 3, y, std::u32string(1, '0' + getValue()), RasterFont::simple5(), value_was_given ? Rgb::WHITE : Rgb::BLUE, 3);
		}
		else if (!no_candidates)
		{
			if (candidates_bf & (1 << 0))
			{
				rt.drawText(x + 1, y + 0, U"1", RasterFont::simple5(), Rgb::WHITE, 1);
			}
			if (candidates_bf & (1 << 1))
			{
				rt.drawText(x + 6, y + 0, U"2", RasterFont::simple5(), Rgb::WHITE, 1);
			}
			if (candidates_bf & (1 << 2))
			{
				rt.drawText(x + 11, y + 0, U"3", RasterFont::simple5(), Rgb::WHITE, 1);
			}
			if (candidates_bf & (1 << 3))
			{
				rt.drawText(x + 1, y + 5, U"4", RasterFont::simple5(), Rgb::WHITE, 1);
			}
			if (candidates_bf & (1 << 4))
			{
				rt.drawText(x + 6, y + 5, U"5", RasterFont::simple5(), Rgb::WHITE, 1);
			}
			if (candidates_bf & (1 << 5))
			{
				rt.drawText(x + 11, y + 5, U"6", RasterFont::simple5(), Rgb::WHITE, 1);
			}
			if (candidates_bf & (1 << 6))
			{
				rt.drawText(x + 1, y + 10, U"7", RasterFont::simple5(), Rgb::WHITE, 1);
			}
			if (candidates_bf & (1 << 7))
			{
				rt.drawText(x + 6, y + 10, U"8", RasterFont::simple5(), Rgb::WHITE, 1);
			}
			if (candidates_bf & (1 << 8))
			{
				rt.drawText(x + 11, y + 10, U"9", RasterFont::simple5(), Rgb::WHITE, 1);
			}
		}
	}

	void Sudoku::reset() noexcept
	{
		for (auto& cell : cells)
		{
			cell.reset();
		}
	}

	void Sudoku::setGivenFromString(const char* str) noexcept
	{
		for (index_t y = 0; y != 9; ++y)
		{
			for (index_t x = 0; x != 9; ++x)
			{
				if (*str == '\0')
				{
					break;
				}
				if (*str >= '1' && *str <= '9')
				{
					getCell(x, y).setGiven(*str - '0');
				}
				++str;
			}
		}
	}

	void Sudoku::setGivenFromBinary(const std::string& str)
	{
		StringRefReader r(str);
		BitReader br(&r);
		uint8_t version;
		br.u8(3, version);
		if (version == 0)
		{
			uint8_t pad;
			br.u8(1, pad);
			for (index_t y = 0; y != 9; ++y)
			{
				for (index_t x = 0; x != 9; ++x)
				{
					uint8_t value;
					br.u8(4, value);
					getCell(x, y).setGiven(value);
				}
			}
		}
		else if (version == 1)
		{
			BigBitset<81 / 8> bs{};
			for (index_t y = 0; y != 9; ++y)
			{
				for (index_t x = 0; x != 9; ++x)
				{
					bool on = false;
					br.b(on);
					if (on)
					{
						bs.enable(getCellIndex(x, y));
					}
				}
			}
			for (index_t y = 0; y != 9; ++y)
			{
				for (index_t x = 0; x != 9; ++x)
				{
					if (bs.get(getCellIndex(x, y)))
					{
						uint8_t value;
						br.u8(4, value);
						getCell(x, y).setGiven(value);
					}
				}
			}
		}
		else if (version == 2)
		{
			bool include_zero;
			br.b(include_zero);
			for (index_t y = 0; y != 9; ++y)
			{
				std::vector<value_t> candidates{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
				if (include_zero)
				{
					candidates.insert(candidates.cbegin(), 0);
				}
				for (index_t x = 0; x != 9; ++x)
				{
					uint8_t i = 0;
					if (candidates.size() != 1)
					{
						br.u8(bitutil::getBitsNeededToEncodeRange(candidates.size()), i);
					}
					const value_t value = candidates.at(i);
					getCell(x, y).setGiven(value);
					if (value != 0)
					{
						candidates.erase(candidates.cbegin() + i);
					}
				}
			}
		}
		else
		{
			SOUP_ASSERT_UNREACHABLE;
		}
	}

	count_t Sudoku::getNumValues() const noexcept
	{
		count_t count = 0;
		for (index_t y = 0; y != 9; ++y)
		{
			for (index_t x = 0; x != 9; ++x)
			{
				count += !!getCell(x, y).value_bf;
			}
		}
		return count;
	}

	mask_t Sudoku::getValuesInBox(index_t i) const noexcept
	{
		index_t bx = i % 3;
		index_t by = i / 3;
		return getValuesInBox(bx, by);
	}

	mask_t Sudoku::getValuesInBox(index_t bx, index_t by) const noexcept
	{
		return getCell((bx * 3) + 0, (by * 3) + 0).value_bf
			|  getCell((bx * 3) + 1, (by * 3) + 0).value_bf
			|  getCell((bx * 3) + 2, (by * 3) + 0).value_bf
			|  getCell((bx * 3) + 0, (by * 3) + 1).value_bf
			|  getCell((bx * 3) + 1, (by * 3) + 1).value_bf
			|  getCell((bx * 3) + 2, (by * 3) + 1).value_bf
			|  getCell((bx * 3) + 0, (by * 3) + 2).value_bf
			|  getCell((bx * 3) + 1, (by * 3) + 2).value_bf
			|  getCell((bx * 3) + 2, (by * 3) + 2).value_bf
			;
	}

	mask_t Sudoku::getValuesInRow(index_t y) const noexcept
	{
		mask_t value_bf = 0;
		for (uint8_t x = 0; x != 9; ++x)
		{
			value_bf |= getCell(x, y).value_bf;
		}
		return value_bf;
	}

	mask_t Sudoku::getValuesInColumn(index_t x) const noexcept
	{
		mask_t value_bf = 0;
		for (uint8_t y = 0; y != 9; ++y)
		{
			value_bf |= getCell(x, y).value_bf;
		}
		return value_bf;
	}

	mask_t Sudoku::getCandidatesInBox(mask_t value_bf, index_t i) const noexcept
	{
		index_t bx = i % 3;
		index_t by = i / 3;
		return getCandidatesInBox(value_bf, bx, by);
	}

	mask_t Sudoku::getCandidatesInBox(mask_t value_bf, index_t bx, index_t by) const noexcept
	{
		return ((1 << 0) * getCell((bx * 3) + 0, (by * 3) + 0).isCandidateMask(value_bf))
			|  ((1 << 1) * getCell((bx * 3) + 1, (by * 3) + 0).isCandidateMask(value_bf))
			|  ((1 << 2) * getCell((bx * 3) + 2, (by * 3) + 0).isCandidateMask(value_bf))
			|  ((1 << 3) * getCell((bx * 3) + 0, (by * 3) + 1).isCandidateMask(value_bf))
			|  ((1 << 4) * getCell((bx * 3) + 1, (by * 3) + 1).isCandidateMask(value_bf))
			|  ((1 << 5) * getCell((bx * 3) + 2, (by * 3) + 1).isCandidateMask(value_bf))
			|  ((1 << 6) * getCell((bx * 3) + 0, (by * 3) + 2).isCandidateMask(value_bf))
			|  ((1 << 7) * getCell((bx * 3) + 1, (by * 3) + 2).isCandidateMask(value_bf))
			|  ((1 << 8) * getCell((bx * 3) + 2, (by * 3) + 2).isCandidateMask(value_bf))
			;
	}

	mask_t Sudoku::getCandidatesInRow(mask_t value_bf, index_t y) const noexcept
	{
		mask_t candidates_bf = 0;
		for (uint8_t x = 0; x != 9; ++x)
		{
			if (getCell(x, y).isCandidateMask(value_bf))
			{
				candidates_bf |= (1 << x);
			}
		}
		return candidates_bf;
	}

	mask_t Sudoku::getCandidatesInColumn(mask_t value_bf, index_t x) const noexcept
	{
		mask_t candidates_bf = 0;
		for (uint8_t y = 0; y != 9; ++y)
		{
			if (getCell(x, y).isCandidateMask(value_bf))
			{
				candidates_bf |= (1 << y);
			}
		}
		return candidates_bf;
	}

	count_t Sudoku::getNumCandidatesInBox(mask_t value_bf, index_t bx, index_t by) const noexcept
	{
		return getCell((bx * 3) + 0, (by * 3) + 0).isCandidateMask(value_bf)
			+  getCell((bx * 3) + 1, (by * 3) + 0).isCandidateMask(value_bf)
			+  getCell((bx * 3) + 2, (by * 3) + 0).isCandidateMask(value_bf)
			+  getCell((bx * 3) + 0, (by * 3) + 1).isCandidateMask(value_bf)
			+  getCell((bx * 3) + 1, (by * 3) + 1).isCandidateMask(value_bf)
			+  getCell((bx * 3) + 2, (by * 3) + 1).isCandidateMask(value_bf)
			+  getCell((bx * 3) + 0, (by * 3) + 2).isCandidateMask(value_bf)
			+  getCell((bx * 3) + 1, (by * 3) + 2).isCandidateMask(value_bf)
			+  getCell((bx * 3) + 2, (by * 3) + 2).isCandidateMask(value_bf)
			;
	}

	count_t Sudoku::getNumCandidatesInRow(mask_t value_bf, index_t y) const noexcept
	{
		count_t res = 0;
		for (index_t x = 0; x != 9; ++x)
		{
			res += getCell(x, y).isCandidateMask(value_bf);
		}
		return res;
	}

	count_t Sudoku::getNumCandidatesInColumn(mask_t value_bf, index_t x) const noexcept
	{
		count_t res = 0;
		for (index_t y = 0; y != 9; ++y)
		{
			res += getCell(x, y).isCandidateMask(value_bf);
		}
		return res;
	}

	void Sudoku::eliminateImpossibleCandiates() noexcept
	{
		for (index_t y = 0; y != 9; ++y)
		{
			for (index_t x = 0; x != 9; ++x)
			{
				if (getCell(x, y).value_bf != 0)
				{
					getCell(x, y).candidates_bf = 0;
				}
				else
				{
					index_t bx = x / 3;
					index_t by = y / 3;

					getCell(x, y).candidates_bf &= ~getValuesInBox(bx, by);
					getCell(x, y).candidates_bf &= ~getValuesInRow(y);
					getCell(x, y).candidates_bf &= ~getValuesInColumn(x);
				}
			}
		}
	}

	bool Sudoku::eliminateCandidate(mask_t value_bf, index_t x, index_t y) noexcept
	{
		if (getCell(x, y).candidates_bf & value_bf)
		{
			getCell(x, y).candidates_bf &= ~value_bf;
			return true;
		}
		return false;
	}

	static const char* bx_names[] = { "left", "centre", "right" };

	bool Sudoku::narrowCandidatesInRowToBox(mask_t value_bf, index_t y, index_t pin_bx, std::string* explanation)
	{
		bool changed = false;
		for (index_t x = 0; x != 9; ++x)
		{
			index_t bx = x / 3;

			if (bx != pin_bx)
			{
				changed |= eliminateCandidate(value_bf, x, y);
			}
		}
		if (explanation && changed)
		{
			*explanation = format("{} in row {} can only be in {} box.", maskToValue(value_bf), getRowName(y), bx_names[pin_bx]);
		}
		return changed;
	}

	static const char* by_names[] = { "top", "centre", "bottom" };

	bool Sudoku::narrowCandidatesInColumnToBox(mask_t value_bf, index_t x, index_t pin_by, std::string* explanation)
	{
		bool changed = false;
		for (index_t y = 0; y != 9; ++y)
		{
			index_t by = y / 3;

			if (by != pin_by)
			{
				changed |= eliminateCandidate(value_bf, x, y);
			}
		}
		if (explanation && changed)
		{
			*explanation = format("{} in column {} can only be in {} box.", maskToValue(value_bf), getColumnName(x), by_names[pin_by]);
		}
		return changed;
	}

	bool Sudoku::narrowCandidatesInBoxToRow(mask_t value_bf, index_t bx, index_t by, index_t pin_y, std::string* explanation)
	{
		bool changed = false;
		if ((by * 3) + 0 != pin_y)
		{
			changed |= eliminateCandidate(value_bf, (bx * 3) + 0, (by * 3) + 0);
			changed |= eliminateCandidate(value_bf, (bx * 3) + 1, (by * 3) + 0);
			changed |= eliminateCandidate(value_bf, (bx * 3) + 2, (by * 3) + 0);
		}
		if ((by * 3) + 1 != pin_y)
		{
			changed |= eliminateCandidate(value_bf, (bx * 3) + 0, (by * 3) + 1);
			changed |= eliminateCandidate(value_bf, (bx * 3) + 1, (by * 3) + 1);
			changed |= eliminateCandidate(value_bf, (bx * 3) + 2, (by * 3) + 1);
		}
		if ((by * 3) + 2 != pin_y)
		{
			changed |= eliminateCandidate(value_bf, (bx * 3) + 0, (by * 3) + 2);
			changed |= eliminateCandidate(value_bf, (bx * 3) + 1, (by * 3) + 2);
			changed |= eliminateCandidate(value_bf, (bx * 3) + 2, (by * 3) + 2);
		}
		if (explanation && changed)
		{
			*explanation = format("{} in {}, {} box can only be in row {}.", maskToValue(value_bf), by_names[by], bx_names[bx], getRowName(pin_y));
		}
		return changed;
	}

	bool Sudoku::narrowCandidatesInBoxToColumn(mask_t value_bf, index_t bx, index_t by, index_t pin_x, std::string* explanation)
	{
		bool changed = false;
		if ((bx * 3) + 0 != pin_x)
		{
			changed |= eliminateCandidate(value_bf, (bx * 3) + 0, (by * 3) + 0);
			changed |= eliminateCandidate(value_bf, (bx * 3) + 0, (by * 3) + 1);
			changed |= eliminateCandidate(value_bf, (bx * 3) + 0, (by * 3) + 2);
		}
		if ((bx * 3) + 1 != pin_x)
		{
			changed |= eliminateCandidate(value_bf, (bx * 3) + 1, (by * 3) + 0);
			changed |= eliminateCandidate(value_bf, (bx * 3) + 1, (by * 3) + 1);
			changed |= eliminateCandidate(value_bf, (bx * 3) + 1, (by * 3) + 2);
		}
		if ((bx * 3) + 2 != pin_x)
		{
			changed |= eliminateCandidate(value_bf, (bx * 3) + 2, (by * 3) + 0);
			changed |= eliminateCandidate(value_bf, (bx * 3) + 2, (by * 3) + 1);
			changed |= eliminateCandidate(value_bf, (bx * 3) + 2, (by * 3) + 2);
		}
		if (explanation && changed)
		{
			*explanation = format("{} in {}, {} box can only be in column {}.", maskToValue(value_bf), by_names[by], bx_names[bx], getColumnName(pin_x));
		}
		return changed;
	}

	bool Sudoku::eliminateCandidatesInRow(mask_t value_bf, index_t y, index_t exclude_x1, index_t exclude_x2) noexcept
	{
		bool changed = false;
		for (index_t x = 0; x != 9; ++x)
		{
			if (x != exclude_x1 && x != exclude_x2)
			{
				changed |= eliminateCandidate(value_bf, x, y);
			}
		}
		return changed;
	}

	bool Sudoku::eliminateCandidatesInColumn(mask_t value_bf, index_t x, index_t exclude_y1, index_t exclude_y2) noexcept
	{
		bool changed = false;
		for (index_t y = 0; y != 9; ++y)
		{
			if (y != exclude_y1 && y != exclude_y2)
			{
				changed |= eliminateCandidate(value_bf, x, y);
			}
		}
		return changed;
	}

	bool Sudoku::isSolvable() const noexcept
	{
		// Rows
		for (index_t y = 0; y != 9; ++y)
		{
			const auto values = getValuesInRow(y);
			for (value_t value = 1; value != 10; ++value)
			{
				const auto mask = valueToMask(value);
				if ((values & mask) == 0
					&& !getCandidatesInRow(mask, y)
					)
				{
					return false;
				}
			}
		}

		// Columns
		for (index_t x = 0; x != 9; ++x)
		{
			const auto values = getValuesInColumn(x);
			for (value_t value = 1; value != 10; ++value)
			{
				const auto mask = valueToMask(value);
				if ((values & mask) == 0
					&& !getCandidatesInColumn(mask, x)
					)
				{
					return false;
				}
			}
		}

		// Boxes
		for (index_t i = 0; i != 9; ++i)
		{
			const auto values = getValuesInBox(i);
			for (value_t value = 1; value != 10; ++value)
			{
				const auto mask = valueToMask(value);
				if ((values & mask) == 0
					&& !getCandidatesInBox(mask, i)
					)
				{
					return false;
				}
			}
		}

		return true;
	}

	bool Sudoku::step(uint8_t strategies, std::string* explanation)
	{
		return ((strategies & NAKED_SINGLE) && stepNakedSingle(explanation))
			|| ((strategies & HIDDEN_SINGLE) && stepHiddenSingle(explanation))
			|| ((strategies & LOCKED_CANDIDATES) && stepLockedCandidates(explanation))
			|| ((strategies & HIDDEN_PAIR) && stepHiddenPair(explanation))
			|| ((strategies & X_WING) && stepXWing(explanation))
			|| ((strategies & CONTRADICTION_IF_CANDIDATE_REMOVED) && stepContradictionIfCandidateRemoved(explanation))
			;
	}

	bool Sudoku::stepNakedSingle(std::string* explanation)
	{
		for (index_t i = 0; i != 9 * 9; ++i)
		{
			if (cells[i].getNumCandidates() == 1)
			{
				cells[i].value_bf = cells[i].candidates_bf;
				if (explanation)
				{
					index_t x = i % 9;
					index_t y = i / 9;
					*explanation = format("{} can only be a {}.", getCellName(x, y), cells[i].getValue());
				}
				return true;
			}
		}
		return false;
	}

	bool Sudoku::stepHiddenSingle(std::string* explanation)
	{
		for (index_t y = 0; y != 9; ++y)
		{
			for (index_t x = 0; x != 9; ++x)
			{
				index_t bx = x / 3;
				index_t by = y / 3;

				auto candidates_bf = getCell(x, y).candidates_bf;
				while (candidates_bf)
				{
					value_t value = bitutil::getLeastSignificantSetBit(candidates_bf) + 1;
					const auto value_bf = valueToMask(value);

					if (getNumCandidatesInBox(value_bf, bx, by) == 1)
					{
						getCell(x, y).value_bf = value_bf;
						if (explanation)
						{
							*explanation = format("{} is the only place where a {} can go in the {}, {} box.", getCellName(x, y), getCell(x, y).getValue(), by_names[by], bx_names[bx]);
						}
						return true;
					}
					if (getNumCandidatesInRow(value_bf, y) == 1)
					{
						getCell(x, y).value_bf = value_bf;
						if (explanation)
						{
							*explanation = format("{} is the only place where a {} can go in row {}.", getCellName(x, y), getCell(x, y).getValue(), getRowName(y));
						}
						return true;
					}
					if (getNumCandidatesInColumn(value_bf, x) == 1)
					{
						getCell(x, y).value_bf = value_bf;
						if (explanation)
						{
							*explanation = format("{} is the only place where a {} can go in column {}.", getCellName(x, y), getCell(x, y).getValue(), getColumnName(x));
						}
						return true;
					}

					bitutil::unsetLeastSignificantSetBit(candidates_bf);
				}
			}
		}
		return false;
	}

	bool Sudoku::stepLockedCandidates(std::string* explanation)
	{
		for (value_t value = 1; value != 10; ++value)
		{
			const auto value_bf = valueToMask(value);

			// Type 1
			for (index_t by = 0; by != 3; ++by)
			{
				for (index_t bx = 0; bx != 3; ++bx)
				{
					const auto candidates = getCandidatesInBox(value_bf, bx, by);
					if (((candidates & 0b000'000'111) != 0)
						&& ((candidates & 0b111'111'000) == 0)
						)
					{
						if (narrowCandidatesInRowToBox(value_bf, (by * 3) + 0, bx, explanation))
						{
							return true;
						}
					}
					else if (((candidates & 0b000'111'000) != 0)
						&& ((candidates & 0b111'000'111) == 0)
						)
					{
						if (narrowCandidatesInRowToBox(value_bf, (by * 3) + 1, bx, explanation))
						{
							return true;
						}
					}
					else if (((candidates & 0b111'000'000) != 0)
						&& ((candidates & 0b000'111'111) == 0)
						)
					{
						if (narrowCandidatesInRowToBox(value_bf, (by * 3) + 2, bx, explanation))
						{
							return true;
						}
					}
					else if (((candidates & 0b001'001'001) != 0)
						&& ((candidates & 0b110'110'110) == 0)
						)
					{
						if (narrowCandidatesInColumnToBox(value_bf, (bx * 3) + 0, by, explanation))
						{
							return true;
						}
					}
					else if (((candidates & 0b010'010'010) != 0)
						&& ((candidates & 0b101'101'101) == 0)
						)
					{
						if (narrowCandidatesInColumnToBox(value_bf, (bx * 3) + 1, by, explanation))
						{
							return true;
						}
					}
					else if (((candidates & 0b100'100'100) != 0)
						&& ((candidates & 0b011'011'011) == 0)
						)
					{
						if (narrowCandidatesInColumnToBox(value_bf, (bx * 3) + 2, by, explanation))
						{
							return true;
						}
					}
				}
			}

			// Type 2
			for (index_t y = 0; y != 9; ++y)
			{
				index_t by = y / 3;

				auto candidates = getCandidatesInRow(value_bf, y);
				if (((candidates & 0b000'000'111) != 0)
					&& ((candidates & 0b111'111'000) == 0)
					)
				{
					if (narrowCandidatesInBoxToRow(value_bf, 0, by, y, explanation))
					{
						return true;
					}
				}
				else if (((candidates & 0b000'111'000) != 0)
					&& ((candidates & 0b111'000'111) == 0)
					)
				{
					if (narrowCandidatesInBoxToRow(value_bf, 1, by, y, explanation))
					{
						return true;
					}
				}
				else if (((candidates & 0b111'000'000) != 0)
					&& ((candidates & 0b000'111'111) == 0)
					)
				{
					if (narrowCandidatesInBoxToRow(value_bf, 2, by, y, explanation))
					{
						return true;
					}
				}
			}

			for (index_t x = 0; x != 9; ++x)
			{
				index_t bx = x / 3;

				auto candidates = getCandidatesInColumn(value_bf, x);
				if (((candidates & 0b000'000'111) != 0)
					&& ((candidates & 0b111'111'000) == 0)
					)
				{
					if (narrowCandidatesInBoxToColumn(value_bf, bx, 0, x, explanation))
					{
						return true;
					}
				}
				else if (((candidates & 0b000'111'000) != 0)
					&& ((candidates & 0b111'000'111) == 0)
					)
				{
					if (narrowCandidatesInBoxToColumn(value_bf, bx, 1, x, explanation))
					{
						return true;
					}
				}
				else if (((candidates & 0b111'000'000) != 0)
					&& ((candidates & 0b000'111'111) == 0)
					)
				{
					if (narrowCandidatesInBoxToColumn(value_bf, bx, 2, x, explanation))
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	bool Sudoku::stepHiddenPair(std::string* explanation)
	{
		for (value_t value1 = 1; value1 != 10; ++value1)
		{
			for (value_t value2 = 1; value2 != 10; ++value2)
			{
				if (value1 != value2)
				{
					const mask_t value1_bf = valueToMask(value1);
					const mask_t value2_bf = valueToMask(value2);

					// Row
					for (index_t y = 0; y != 9; ++y)
					{
						auto candidates = getCandidatesInRow(value1_bf, y);
						if (candidates == getCandidatesInRow(value2_bf, y)
							&& bitutil::getNumSetBits(candidates) == 2
							)
						{
							//std::cout << "Found hidden pair on " << (int)value1 << " and " << (int)value2 << " in row " << (y + 1) << "\n";
							bool changed = false;
							do
							{
								index_t x = bitutil::getLeastSignificantSetBit(candidates);
								changed |= eliminateCandidate(~(value1_bf | value2_bf), x, y);
							} while (bitutil::unsetLeastSignificantSetBit(candidates), candidates);
							if (changed)
							{
								if (explanation)
								{
									*explanation = format("There's a hidden pair on {}s and {}s in row {}.", value1, value2, getRowName(y));
								}
								return true;
							}
						}
					}

					// Column
					for (index_t x = 0; x != 9; ++x)
					{
						auto candidates = getCandidatesInColumn(value1_bf, x);
						if (candidates == getCandidatesInColumn(value2_bf, x)
							&& bitutil::getNumSetBits(candidates) == 2
							)
						{
							//std::cout << "Found hidden pair on " << (int)value1 << " and " << (int)value2 << " in column " << (x + 1) << "\n";
							bool changed = false;
							do
							{
								index_t y = bitutil::getLeastSignificantSetBit(candidates);
								changed |= eliminateCandidate(~(value1_bf | value2_bf), x, y);
							} while (bitutil::unsetLeastSignificantSetBit(candidates), candidates);
							if (changed)
							{
								if (explanation)
								{
									*explanation = format("There's a hidden pair on {}s and {}s in column {}.", value1, value2, getColumnName(x));
								}
								return true;
							}
						}
					}

					// Could technically also do this for boxes although I think row & column is enough to pick everything up.
				}
			}
		}
		return false;
	}

	bool Sudoku::stepXWing(std::string* explanation)
	{
		for (value_t value = 1; value != 10; ++value)
		{
			const auto value_bf = valueToMask(value);

			// Check rows to eliminate candidates in respective columns
			for (index_t r1y = 0; r1y != 9; ++r1y)
			{
				const auto c_candidates = getCandidatesInRow(value_bf, r1y);
				if (bitutil::getNumSetBits(c_candidates) == 2)
				{
					for (index_t r2y = 0; r2y != 9; ++r2y)
					{
						if (r1y != r2y
							&& c_candidates == getCandidatesInRow(value_bf, r2y)
							)
						{
							bool changed = false;
							auto candidates = c_candidates;
							do
							{
								index_t x = bitutil::getLeastSignificantSetBit(candidates);
								changed |= eliminateCandidatesInColumn(value_bf, x, r1y, r2y);
							} while (bitutil::unsetLeastSignificantSetBit(candidates), candidates);
							if (changed)
							{
								if (explanation)
								{
									*explanation = format("There's an X-Wing on {}s in row {} and {}.", value, getRowName(r1y), getRowName(r2y));
								}
								return true;
							}
						}
					}
				}
			}

			// Check columns to eliminate candidates in respective rows
			for (index_t c1x = 0; c1x != 9; ++c1x)
			{
				const auto c_candidates = getCandidatesInColumn(value_bf, c1x);
				if (bitutil::getNumSetBits(c_candidates) == 2)
				{
					for (index_t c2x = 0; c2x != 9; ++c2x)
					{
						if (c1x != c2x
							&& c_candidates == getCandidatesInColumn(value_bf, c2x)
							)
						{
							bool changed = false;
							auto candidates = c_candidates;
							do 
							{
								index_t y = bitutil::getLeastSignificantSetBit(candidates);
								changed |= eliminateCandidatesInRow(value_bf, y, c1x, c2x);
							} while (bitutil::unsetLeastSignificantSetBit(candidates), candidates);
							if (changed)
							{
								if (explanation)
								{
									*explanation = format("There's an X-Wing on {}s in column {} and {}.", value, getColumnName(c1x), getColumnName(c2x));
								}
								return true;
							}
						}
					}
				}
			}
		}
		return false;
	}

	// Needed to make progress in this situation: 870406325040300170003700409610038704000104030384675291490003007020000043536847912 (from this puzzle: 800006305040000070000000000010038704000104000300070290000003000020000040506800002)
	bool Sudoku::stepContradictionIfCandidateRemoved(std::string* explanation)
	{
		SOUP_ASSERT(isSolvable());
		for (value_t value = 1; value != 10; ++value)
		{
			const auto value_bf = valueToMask(value);

			// Only checking rows for candidates because chains are usually across rows, columns, and boxes, so I think checking any would yield all.
			for (index_t y = 0; y != 9; ++y)
			{
				auto candidates = getCandidatesInRow(value_bf, y);
				if (bitutil::getNumSetBits(candidates) == 2)
				{
					do
					{
						index_t x = bitutil::getLeastSignificantSetBit(candidates);
						Sudoku cpy(*this);
						cpy.getCell(x, y).candidates_bf &= ~value_bf;
						while (cpy.stepNakedSingle() || cpy.stepHiddenSingle())
						{
							cpy.eliminateImpossibleCandiates();
						}
						if (!cpy.isSolvable())
						{
							// Removing the candidate from the cell broke the Sudoku. Therefore, the candidate is the only option for the cell.
							getCell(x, y).candidates_bf = value_bf;
							if (explanation)
							{
								*explanation = format("There are a lot of {}s chained together. If we imagine that the {} in {} were removed, we'd find that the chain implies there must be a {} there.", value, value, getCellName(x, y), value);
							}
							return true;
						}
					} while (bitutil::unsetLeastSignificantSetBit(candidates), candidates);
				}
			}
		}
		return false;
	}

	bool Sudoku::solveCell(std::string* explanation)
	{
		if (explanation)
		{
			eliminateImpossibleCandiates();
			if (!stepNakedSingle(explanation) && !stepHiddenSingle(explanation))
			{
				std::vector<std::string> steps;
				do
				{
					if (!step(ALL, &steps.emplace_back()))
					{
						return false;
					}
					eliminateImpossibleCandiates();
				}
				while (!stepNakedSingle(explanation) && !stepHiddenSingle(explanation));
				explanation->insert(0, "Now, we can see ");
				for (auto i = steps.rbegin(); i != steps.rend(); ++i)
				{
					explanation->insert(0, 1, ' ');
					explanation->insert(0, *i);
				}
			}
			return true;
		}
		else
		{
			const auto values = getNumValues();
			do
			{
				if (getNumValues() != values)
				{
					return true;
				}
				eliminateImpossibleCandiates();
			} while (step());
			return false;
		}
	}

	bool Sudoku::canSolve(uint8_t strategies) const
	{
		Sudoku cpy = *this;
		cpy.solve(strategies);
		return cpy.getNumValues() == COUNT(cells);
	}

	void Sudoku::draw(RenderTarget& rt, bool no_candidates) const
	{
		for (index_t y = 0; y != 9; ++y)
		{
			for (index_t x = 0; x != 9; ++x)
			{
				getCell(x, y).draw(rt, no_candidates, x * 15, y * 15);
			}
		}
	}

	void Sudoku::fill()
	{
		do
		{
			eliminateImpossibleCandiates();
		} while (stepNakedSingle() || stepHiddenSingle());
		Sudoku cpy = *this;
	_retry:;
		for (auto& c : cells)
		{
			if (c.value_bf)
			{
				continue;
			}
			auto bf = c.candidates_bf;
			uint32_t bits = bitutil::getNumSetBits(bf);
			if (bits == 0)
			{
				// Oops, this cell has no candidates.
				*this = cpy;
				goto _retry;
			}
			int32_t bit = soup::rand.t<int32_t>(0, bits - 1);
			while (bit--)
			{
				bitutil::unsetLeastSignificantSetBit(bf);
			}
			c.setGiven(bitutil::getLeastSignificantSetBit(bf) + 1);
			do
			{
				eliminateImpossibleCandiates();
			} while (stepNakedSingle() || stepHiddenSingle());
		}
	}

	void Sudoku::removeRandomDigit()
	{
		const count_t size = getNumValues();
		if (size != 0)
		{
			const auto to_erase = soup::rand.t<count_t>(0, size - 1);
			count_t i = 0;
			for (auto& cell : cells)
			{
				if (cell.value_bf)
				{
					if (++i == to_erase)
					{
						cell.reset();
						break;
					}
				}
			}
		}
	}

	std::string Sudoku::toString() const
	{
		std::string str{};
		str.reserve(9 * 9);
		for (index_t y = 0; y != 9; ++y)
		{
			for (index_t x = 0; x != 9; ++x)
			{
				str.push_back('0' + getCell(x, y).getValue());
			}
		}
		return str;
	}

	std::string Sudoku::toBinary() const
	{
		auto v2 = toBinaryV2();
		auto v1 = toBinaryV1();
		if (v2.length() > v1.length())
		{
			return v1;
		}
		return v2;
	}

	std::string Sudoku::toBinaryV0() const
	{
		StringWriter w;
		BitWriter bw(&w);
		bw.u8(3, 0); // version
		bw.u8(1, 0); // pad
		for (index_t y = 0; y != 9; ++y)
		{
			for (index_t x = 0; x != 9; ++x)
			{
				bw.u8(4, getCell(x, y).getValue());
			}
		}
#ifdef _DEBUG
		SOUP_ASSERT(bw.isByteAligned());
#endif
		return w.data;
	}

	std::string Sudoku::toBinaryV1() const
	{
		StringWriter w;
		BitWriter bw(&w);
		bw.u8(3, 1);
		for (index_t y = 0; y != 9; ++y)
		{
			for (index_t x = 0; x != 9; ++x)
			{
				bw.b(!!getCell(x, y).value_bf);
			}
		}
		for (index_t y = 0; y != 9; ++y)
		{
			for (index_t x = 0; x != 9; ++x)
			{
				if (getCell(x, y).value_bf)
				{
					bw.u8(4, getCell(x, y).getValue());
				}
			}
		}
		bw.finishByte();
		return w.data;
	}

	[[nodiscard]] static index_t value2index(const std::vector<value_t>& candidates, value_t val)
	{
		for (index_t i = 0; i != candidates.size(); ++i)
		{
			if (candidates[i] == val)
			{
				return i;
			}
		}
		SOUP_ASSERT_UNREACHABLE;
	}

	std::string Sudoku::toBinaryV2() const
	{
		bool include_zero = false;
		for (const auto& cell : cells)
		{
			if (cell.value_bf == 0)
			{
				include_zero = true;
				break;
			}
		}

		StringWriter w;
		BitWriter bw(&w);
		bw.u8(3, 2);
		bw.b(include_zero);
		for (index_t y = 0; y != 9; ++y)
		{
			std::vector<value_t> candidates{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			if (include_zero)
			{
				candidates.insert(candidates.cbegin(), 0);
			}
			for (index_t x = 0; x != 9; ++x)
			{
				if (candidates.size() == 1)
				{
					break;
				}
				const value_t value = getCell(x, y).getValue();
				index_t i = value2index(candidates, value);
				bw.u8(bitutil::getBitsNeededToEncodeRange(candidates.size()), i);
				if (value != 0)
				{
					candidates.erase(candidates.cbegin() + i);
				}
			}
		}
		bw.finishByte();
		return w.data;
	}
}
