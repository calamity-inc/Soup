#pragma once

#include "stdint.hpp"

#include <string>
#include <utility>
#include <vector>

namespace soup
{
	class bigint
	{
	public:
		using chunk_t = halfsize_t;
		using chunk_signed_t = halfintmax_t;

	private:
		std::vector<chunk_t> chunks{};
		bool negative = false;

	public:
		bigint() noexcept = default;
		bigint(chunk_signed_t v);
		bigint(chunk_t v, bool negative = false);
		bigint(intmax_t v);
		bigint(size_t v, bool negative = false);
		bigint(bigint&& b);
		bigint(const bigint& b);

		[[nodiscard]] static uint8_t getBytesPerChunk() noexcept;
		[[nodiscard]] static uint8_t getNibblesPerChunk() noexcept;
		[[nodiscard]] static uint8_t getBitsPerChunk() noexcept;

		[[nodiscard]] static chunk_t getCarry(size_t v) noexcept;
		
		[[nodiscard]] size_t getNumChunks() const noexcept;
		[[nodiscard]] chunk_t getChunk(size_t i) const noexcept;
		void setChunk(size_t i, chunk_t v);
		void addChunk(chunk_t v);
		void shrink() noexcept;

		[[nodiscard]] size_t getNumBytes() const noexcept;
		[[nodiscard]] uint8_t getByte(size_t i) const noexcept;

		[[nodiscard]] size_t getNumNibbles() const noexcept;
		[[nodiscard]] uint8_t getNibble(size_t i) const noexcept;

		[[nodiscard]] size_t getNumBits() const noexcept;
		[[nodiscard]] bool getBit(size_t i) const noexcept;
		void setBit(size_t i, bool v);
		void enableBit(size_t i);
		void disableBit(size_t i);

		[[nodiscard]] bool isZero() const noexcept;
		void reset() noexcept;

		[[nodiscard]] int cmp(const bigint& b) const noexcept;
		[[nodiscard]] bool operator == (const bigint& b) const noexcept;
		[[nodiscard]] bool operator != (const bigint& b) const noexcept;
		[[nodiscard]] bool operator > (const bigint& b) const noexcept;
		[[nodiscard]] bool operator >= (const bigint& b) const noexcept;
		[[nodiscard]] bool operator < (const bigint& b) const noexcept;
		[[nodiscard]] bool operator <= (const bigint& b) const noexcept;

		void operator=(chunk_signed_t v);
		void operator=(chunk_t v);
		void operator=(intmax_t v);
		void operator=(size_t v);
	private:
		void setChunks(chunk_t v);
		void setChunks(size_t v);
	public:
		void operator=(bigint&& b);
		void operator=(const bigint& b);

		void operator+=(const bigint& b);
		void operator-=(const bigint& subtrahend);
		void operator*=(const bigint& b);
		std::pair<bigint, bigint> divide(const bigint& divisor) const; // (Quotient, Remainder)
		void operator/=(const bigint& b);
		void operator%=(const bigint& b);
		void operator<<=(size_t b);
		void operator>>=(size_t b);
		
		[[nodiscard]] bigint operator+(const bigint& b) const;
		void operator++();
		[[nodiscard]] bigint operator++(int);
		[[nodiscard]] bigint operator-(const bigint& subtrahend) const;
		void operator--();
		[[nodiscard]] bigint operator--(int);
		[[nodiscard]] bigint operator*(const bigint& b) const;
		[[nodiscard]] bigint operator/(const bigint& b) const;
		[[nodiscard]] bigint operator%(const bigint& b) const;
		[[nodiscard]] bigint operator<<(size_t b) const;
		[[nodiscard]] bigint operator>>(size_t b) const;

		bool toPrimitive(size_t& out) const noexcept;

		template <typename Str = std::string>
		[[nodiscard]] Str toStringDecimal() const noexcept
		{
			Str str{};
			bigint quotient(*this);
			quotient.negative = false;
			if (quotient.isZero())
			{
				str.append(1, '0');
			}
			else do
			{
				auto res = quotient.divide(10u);
				str.insert(0, 1, '0' + res.second.getChunk(0));
				quotient = std::move(res.first);
			} while (!quotient.isZero());
			if (negative)
			{
				str.insert(0, 1, '-');
			}
			return str;
		}

		template <typename Str = std::string>
		[[nodiscard]] Str toStringBinary(bool prefix = false) const noexcept
		{
			size_t i = getNumBits();
			// skip leading zeroes
			while (i-- != 0 && !getBit(i));
			Str str{};
			str.reserve(i + 1 + (prefix * 2) + negative);
			do
			{
				str.push_back('0' + getBit(i));
			} while (i-- != 0);
			if (prefix)
			{
				str.insert(0, 1, 'b');
				str.insert(0, 1, '0');
			}
			if (negative)
			{
				str.insert(0, 1, '-');
			}
			return str;
		}

		[[nodiscard]] std::string toStringHexUpper(bool prefix = false) const noexcept;
		[[nodiscard]] std::string toStringHexLower(bool prefix = false) const noexcept;
	private:
		[[nodiscard]] std::string toStringHexImpl(bool prefix, const char* map) const noexcept;
	};

	namespace literals
	{
		inline bigint operator "" _b(unsigned long long v)
		{
			return bigint((size_t)v);
		}
	}
}
