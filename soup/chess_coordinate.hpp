#pragma once

#include <cstdint>
#include <string>

namespace soup
{
	struct chess_coordinate
	{
		uint8_t file;
		uint8_t rank;

		chess_coordinate() noexcept
			: file(8), rank(0)
		{
		}

		chess_coordinate(uint8_t file, uint8_t rank) noexcept
			: file(file), rank(rank)
		{
		}

		chess_coordinate(const std::string& str)
			: chess_coordinate(str.at(0) - 'a', str.at(1) - '1')
		{
		}

		chess_coordinate(const char* str)
			: chess_coordinate(std::string(str))
		{
		}

		[[nodiscard]] bool operator==(const chess_coordinate& b) const noexcept
		{
			return file == b.file && rank == b.rank;
		}

		[[nodiscard]] bool operator!=(const chess_coordinate& b) const noexcept
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
