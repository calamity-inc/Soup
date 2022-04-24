#pragma once

#include <cstdint>
#include <string>

namespace soup
{
	struct ChessCoordinate
	{
		uint8_t file;
		uint8_t rank;

		ChessCoordinate() noexcept
			: file(8), rank(0)
		{
		}

		ChessCoordinate(uint8_t file, uint8_t rank) noexcept
			: file(file), rank(rank)
		{
		}

		ChessCoordinate(const std::string& str)
			: ChessCoordinate(str.at(0) - 'a', str.at(1) - '1')
		{
		}

		ChessCoordinate(const char* str)
			: ChessCoordinate(std::string(str))
		{
		}

		[[nodiscard]] bool operator==(const ChessCoordinate& b) const noexcept
		{
			return file == b.file && rank == b.rank;
		}

		[[nodiscard]] bool operator!=(const ChessCoordinate& b) const noexcept
		{
			return !operator==(b);
		}

		[[nodiscard]] bool isValid() const noexcept
		{
			// file < 8 && rank < 8
			return ((file >> 3) | (rank >> 3)) == 0;
		}

		void invalidate() noexcept
		{
			file = 8;
		}

		[[nodiscard]] std::string toString() const noexcept
		{
			std::string str{};
			str.reserve(2);
			str.push_back('a' + file);
			str.push_back('1' + rank);
			return str;
		}
	};
}
