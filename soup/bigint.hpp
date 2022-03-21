#pragma once

#include "base.hpp"

#include <ostream>
#include <string>
#include <utility> // pair

#include "int_vector.hpp"

namespace soup
{
	class bigint
	{
	public:
		using chunk_t = halfsize_t;
		using chunk_signed_t = halfintmax_t;

	private:
		int_vector<chunk_t> chunks{};
		bool negative = false;

	public:
		bigint() noexcept = default;
		bigint(chunk_signed_t v);
		bigint(chunk_t v, bool negative = false);
		bigint(intmax_t v);
		bigint(size_t v, bool negative = false);
		bigint(bigint&& b);
		bigint(const bigint& b);

		static const bigint ONE;
		static const bigint TWO;
		static const bigint THREE;
		static const bigint FIVE;
		static const bigint SIX;
		static const bigint TEN;

		[[nodiscard]] static bigint fromString(const char* str, size_t len);
		[[nodiscard]] static bigint fromStringHex(const char* str, size_t len);
	private:
		void fromStringImplBinary(const char* str, size_t len);
		void fromStringImplDecimal(const char* str, size_t len);
		void fromStringImplHex(const char* str, size_t len);

	public:
		[[nodiscard]] static bigint random(size_t bits);
		[[nodiscard]] static bigint randomProbablePrime(const size_t bits);

		[[nodiscard]] static constexpr uint8_t getBitsPerChunk() noexcept
		{
			return SOUP_BITS / 2;
		}

		[[nodiscard]] static constexpr uint8_t getBytesPerChunk() noexcept
		{
			return getBitsPerChunk() / 8;
		}

		[[nodiscard]] static constexpr uint8_t getNibblesPerChunk() noexcept
		{
			return getBytesPerChunk() * 2;
		}

		[[nodiscard]] static constexpr chunk_t getCarry(size_t v) noexcept
		{
			return (chunk_t)(v >> getBitsPerChunk());
		}
		
		[[nodiscard]] constexpr size_t getNumChunks() const noexcept
		{
			return chunks.size();
		}

		[[nodiscard]] chunk_t getChunk(size_t i) const noexcept;
	private:
		[[nodiscard]] chunk_t getChunkInbounds(size_t i) const noexcept;
	public:
		void setChunk(size_t i, chunk_t v);
	private:
		void setChunkInbounds(size_t i, chunk_t v);
	public:
		void addChunk(size_t i, chunk_t v);
		void addChunk(chunk_t v);
		void shrink() noexcept;

		[[nodiscard]] size_t getNumBytes() const noexcept;
		[[nodiscard]] uint8_t getByte(const size_t i) const noexcept;

		[[nodiscard]] size_t getNumNibbles() const noexcept;
		[[nodiscard]] uint8_t getNibble(size_t i) const noexcept;

		[[nodiscard]] size_t getNumBits() const noexcept;
		[[nodiscard]] bool getBit(const size_t i) const noexcept;
		void setBit(const size_t i, const bool v);
		void enableBit(const size_t i);
		void disableBit(const size_t i);
		[[nodiscard]] size_t getBitLength() const noexcept;
		[[nodiscard]] size_t getLowestSetBit() const noexcept;
	private:
		[[nodiscard]] bool getBitInbounds(const size_t i) const noexcept;
		void setBitInbounds(const size_t i, const bool v);
		void enableBitInbounds(const size_t i);
		void disableBitInbounds(const size_t i);

	public:
		void reset() noexcept;
		[[nodiscard]] bool isZero() const noexcept;
		[[nodiscard]] operator bool() const noexcept;

		[[nodiscard]] int cmp(const bigint& b) const noexcept;
		[[nodiscard]] int cmpUnsigned(const bigint& b) const noexcept;
		[[nodiscard]] bool operator == (const bigint& b) const noexcept;
		[[nodiscard]] bool operator != (const bigint& b) const noexcept;
		[[nodiscard]] bool operator > (const bigint& b) const noexcept;
		[[nodiscard]] bool operator >= (const bigint& b) const noexcept;
		[[nodiscard]] bool operator < (const bigint& b) const noexcept;
		[[nodiscard]] bool operator <= (const bigint& b) const noexcept;

		[[nodiscard]] bool operator == (const chunk_t v) const noexcept;
		[[nodiscard]] bool operator != (const chunk_t v) const noexcept;
		[[nodiscard]] bool operator < (const chunk_t v) const noexcept;
		[[nodiscard]] bool operator <= (const chunk_t v) const noexcept;

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
		void addUnsigned(const bigint& b);
		void operator-=(const bigint& subtrahend);
		void subUnsigned(const bigint& subtrahend);
		void operator*=(const bigint& b);
		void operator/=(const bigint& divisor);
		void operator%=(const bigint& divisor);
		[[nodiscard]] std::pair<bigint, bigint> divide(const bigint& divisor) const; // (Quotient, Remainder)
		[[nodiscard]] std::pair<bigint, bigint> divideUnsigned(const bigint& divisor) const; // (Quotient, Remainder)
		[[nodiscard]] bigint modUnsigned(const bigint& divisor) const;
		[[nodiscard]] bigint modUnsignedNotpowerof2(const bigint& divisor) const;
		[[nodiscard]] bool isDivisorOf(const bigint& dividend) const;
		void operator<<=(const size_t b);
	private:
		void leftShiftSmall(const size_t b);
	public:
		void operator>>=(const size_t b);
		void operator|=(const bigint& b);
		void operator&=(const bigint& b);

		[[nodiscard]] bigint operator+(const bigint& b) const;
		bigint& operator++();
		[[nodiscard]] bigint operator++(int);
		[[nodiscard]] bigint operator-(const bigint& subtrahend) const;
		bigint& operator--();
		[[nodiscard]] bigint operator--(int);
		[[nodiscard]] bigint operator*(const bigint& b) const;
		[[nodiscard]] bigint modMulUnsigned(const bigint& b, const bigint& m) const;
		[[nodiscard]] bigint modMulUnsignedNotpowerof2(const bigint& b, const bigint& m) const;
		[[nodiscard]] bigint operator/(const bigint& b) const;
		[[nodiscard]] bigint operator%(const bigint& b) const;
		[[nodiscard]] bigint operator<<(size_t b) const;
		[[nodiscard]] bigint operator>>(size_t b) const;
		[[nodiscard]] bigint operator|(const bigint& b) const;
		[[nodiscard]] bigint operator&(const bigint& b) const;

		[[nodiscard]] bigint abs() const;
		[[nodiscard]] bigint pow(bigint e) const;
		[[nodiscard]] bigint pow2() const;
		[[nodiscard]] bigint modPow(bigint e, const bigint& m) const;
		[[nodiscard]] size_t getTrailingZeroes(const bigint& base) const;
		[[nodiscard]] size_t getTrailingZeroesBinary() const;
		[[nodiscard]] bigint gcd(bigint v) const;
		[[nodiscard]] bigint gcd(bigint b, bigint& x, bigint& y) const;
	private:
		[[nodiscard]] bool isPrimePrecheck(bool& ret) const;
	public:
		[[nodiscard]] bool isPrime() const;
		[[nodiscard]] bool isProbablePrimeMillerRabin(const int iterations = 1) const;
	protected:
		[[nodiscard]] bool isProbablePrimeMillerRabinNoprecheck(const int iterations = 1) const;
	public:
		[[nodiscard]] bool isProbablePrimeFermat(const int iterations = 1) const;
		[[nodiscard]] bool isCoprime(const bigint& b) const;
		[[nodiscard]] bigint eulersTotient() const;
		[[nodiscard]] bigint reducedTotient() const;
		[[nodiscard]] bigint modMulInv(const bigint& m) const;
		[[nodiscard]] bigint lcm(const bigint& b) const;
		[[nodiscard]] bool isPowerOf2() const;

		bool toPrimitive(size_t& out) const;

		template <typename Str = std::string>
		[[nodiscard]] Str toString() const
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
				auto res = quotient.divide((chunk_t)10u);
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
		[[nodiscard]] Str toStringBinary(bool prefix = false) const
		{
			Str str{};
			size_t i = getNumBits();
			if (i == 0)
			{
				str.push_back('0');
			}
			else
			{
				// skip leading zeroes
				while (i-- != 0 && !getBit(i));
				str.reserve(i + 1 + (prefix * 2) + negative);
				do
				{
					str.push_back('0' + getBit(i));
				} while (i-- != 0);
			}
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

		[[nodiscard]] std::string toStringHex(bool prefix = false) const;
		[[nodiscard]] std::string toStringHexLower(bool prefix = false) const;
	private:
		[[nodiscard]] std::string toStringHexImpl(bool prefix, const char* map) const;

	public:
		friend std::ostream& operator<<(std::ostream& os, const bigint& v);

		[[nodiscard]] static bigint fromBinary(const std::string& msg);
		[[nodiscard]] std::string toBinary() const;
	};

	namespace literals
	{
		inline bigint operator "" _b(unsigned long long v)
		{
			return bigint((size_t)v);
		}

		inline bigint operator "" _b(const char* str, size_t len)
		{
			return bigint::fromString(str, len);
		}
	}
}
