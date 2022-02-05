#include "bigint.hpp"

#include "string.hpp"

namespace soup
{
	using chunk_t = bigint::chunk_t;
	using chunk_signed_t = bigint::chunk_signed_t;

	bigint::bigint(chunk_signed_t v)
		: bigint()
	{
		operator =(v);
	}

	bigint::bigint(chunk_t v, bool negative)
		: negative(negative)
	{
		setChunks(v);
	}

	bigint::bigint(intmax_t v)
		: bigint()
	{
		operator =(v);
	}

	bigint::bigint(size_t v, bool negative)
		: negative(negative)
	{
		setChunks(v);
	}

	bigint::bigint(bigint&& b)
		: chunks(std::move(b.chunks)), negative(b.negative)
	{
	}

	bigint::bigint(const bigint& b)
		: chunks(b.chunks), negative(b.negative)
	{
	}

	uint8_t bigint::getBytesPerChunk() noexcept
	{
		return getBitsPerChunk() / 8;
	}

	uint8_t bigint::getNibblesPerChunk() noexcept
	{
		return getBytesPerChunk() * 2;
	}

	uint8_t bigint::getBitsPerChunk() noexcept
	{
		return SOUP_PLATFORM_BITS / 2;
	}

	chunk_t bigint::getCarry(size_t v) noexcept
	{
		return (chunk_t)(v >> getBitsPerChunk());
	}

	size_t bigint::getNumChunks() const noexcept
	{
		return chunks.size();
	}

	chunk_t bigint::getChunk(size_t i) const noexcept
	{
		if (i < chunks.size())
		{
			return chunks.at(i);
		}
		return 0;
	}

	void bigint::setChunk(size_t i, chunk_t v)
	{
		chunks[i] = v;
	}

	void bigint::addChunk(chunk_t v)
	{
		chunks.emplace_back(v);
	}

	void bigint::shrink() noexcept
	{
		for (size_t i = chunks.size(); i-- != 0; )
		{
			auto it = chunks.begin() + i;
			if (*it != 0)
			{
				break;
			}
			chunks.erase(it);
		}
	}

	size_t bigint::getNumBytes() const noexcept
	{
		return getNumChunks() * getBytesPerChunk();
	}

	uint8_t bigint::getByte(size_t i) const noexcept
	{
		const chunk_t chunk = getChunk(i / getBytesPerChunk());
		const auto chunk_bytes = (const uint8_t*)&chunk;
		return chunk_bytes[i % getBytesPerChunk()];
	}

	size_t bigint::getNumNibbles() const noexcept
	{
		return getNumBytes() * 2;
	}

	uint8_t bigint::getNibble(size_t i) const noexcept
	{
		return ((getByte(i / 2) >> ((i % 2) * 4)) & 0b1111);
	}

	size_t bigint::getNumBits() const noexcept
	{
		return getNumChunks() * getBitsPerChunk();
	}

	bool bigint::getBit(size_t i) const noexcept
	{
		return (getChunk(i / getBitsPerChunk()) & (1 << (i % getBitsPerChunk()))) != 0;
	}

	void bigint::setBit(size_t i, bool v)
	{
		if (v)
		{
			enableBit(i);
		}
		else
		{
			disableBit(i);
		}
	}

	void bigint::enableBit(size_t i)
	{
		const auto chunk_i = (i / getBitsPerChunk());
		if (chunk_i < chunks.size())
		{
			chunks.at(chunk_i) |= (1 << (i % getBitsPerChunk()));
		}
		else
		{
			setChunk(chunk_i, (1 << (i % getBitsPerChunk())));
		}
	}

	void bigint::disableBit(size_t i)
	{
		const auto chunk_i = (i / getBitsPerChunk());
		if (chunk_i < chunks.size())
		{
			chunks.at(chunk_i) &= ~(1 << (i % getBitsPerChunk()));
		}
	}

	bool bigint::isZero() const noexcept
	{
		return getNumChunks() == 0;
	}

	void bigint::reset() noexcept
	{
		chunks.clear();
	}

	int bigint::cmp(const bigint& b) const noexcept
	{
		if (getNumChunks() != b.getNumChunks())
		{
			return getNumChunks() > b.getNumChunks() ? +1 : -1;
		}
		size_t i = chunks.size();
		do
		{
			--i;
			if (getChunk(i) != b.getChunk(i))
			{
				return getChunk(i) > b.getChunk(i) ? +1 : -1;
			}
		} while (i != 0);
		return 0;
	}

	bool bigint::operator==(const bigint& b) const noexcept
	{
		return cmp(b) == 0;
	}

	bool bigint::operator!=(const bigint& b) const noexcept
	{
		return cmp(b) != 0;
	}

	bool bigint::operator>(const bigint& b) const noexcept
	{
		return cmp(b) > 0;
	}

	bool bigint::operator>=(const bigint& b) const noexcept
	{
		return cmp(b) >= 0;
	}

	bool bigint::operator<(const bigint& b) const noexcept
	{
		return cmp(b) < 0;
	}

	bool bigint::operator<=(const bigint& b) const noexcept
	{
		return cmp(b) <= 0;
	}

	void bigint::operator=(chunk_signed_t v)
	{
		negative = (v < 0);
		if (negative)
		{
			setChunks((chunk_t)(v * -1));
		}
		else
		{
			setChunks((chunk_t)v);
		}
	}

	void bigint::operator=(chunk_t v)
	{
		setChunks(v);
		negative = false;
	}

	void bigint::operator=(intmax_t v)
	{
		negative = (v < 0);
		if (negative)
		{
			setChunks((size_t)(v * -1));
		}
		else
		{
			setChunks((size_t)v);
		}
	}

	void bigint::operator=(size_t v)
	{
		setChunks(v);
		negative = false;
	}

	void bigint::setChunks(chunk_t v)
	{
		if (v == 0)
		{
			chunks.clear();
		}
		else
		{
			chunks = std::vector<chunk_t>(1, v);
		}
	}

	void bigint::setChunks(size_t v)
	{
		const chunk_t carry = getCarry(v);
		if (carry == 0)
		{
			setChunks((chunk_t)v);
		}
		else
		{
			chunks = std::vector<chunk_t>{ (chunk_t)v, carry };
		}
	}

	void bigint::operator=(bigint&& b)
	{
		chunks = std::move(b.chunks);
		negative = b.negative;
	}

	void bigint::operator=(const bigint& b)
	{
		chunks = b.chunks;
		negative = b.negative;
	}

	void bigint::operator+=(const bigint& b)
	{
		if (*this < b)
		{
			*this = (b + *this);
			return;
		}
		size_t carry = 0;
		const size_t j = getNumChunks();
		for (size_t i = 0; i != j; ++i)
		{
			const size_t x = getChunk(i);
			const size_t y = b.getChunk(i);
			size_t res = (x + y + carry);
			setChunk(i, (chunk_t)res);
			carry = getCarry(res);
		}
		if (carry != 0)
		{
			addChunk(carry);
		}
	}

	void bigint::operator-=(const bigint& subtrahend)
	{
		if (*this < subtrahend)
		{
			chunks = (subtrahend - *this).chunks;
			negative = !isZero();
			return;
		}
		size_t carry = 0;
		const size_t j = getNumChunks();
		for (size_t i = 0; i != j; ++i)
		{
			const size_t x = getChunk(i);
			const size_t y = subtrahend.getChunk(i);
			size_t res = (x - y - carry);
			setChunk(i, (chunk_t)res);
			carry = (getCarry(res) != 0);
		}
		shrink();
	}

	void bigint::operator*=(const bigint& b)
	{
		if (isZero())
		{
			return;
		}
		if (*this < b)
		{
			*this = (b * *this);
			return;
		}
		if (b.isZero())
		{
			reset();
			return;
		}

		if (b.getNumChunks() == 1)
		{
			chunk_t carry = 0;
			const size_t j = getNumChunks();
			const size_t y = b.getChunk(0);
			for (size_t i = 0; i != j; ++i)
			{
				const size_t x = getChunk(i);
				size_t res = ((x * y) + carry);
				setChunk(i, (chunk_t)res);
				carry = getCarry(res);
			}
			if (carry != 0)
			{
				addChunk(carry);
			}
		}
		else
		{
			// FIX ME
			for (bigint i = b; --i, !i.isZero(); )
			{
				*this += *this;
			}
		}
	}

	std::pair<bigint, bigint> bigint::divide(const bigint& divisor) const
	{
		if (divisor.negative)
		{
			bigint divisor_cpy(divisor);
			divisor_cpy.negative = false;
			auto res = divide(divisor_cpy);
			res.first.negative ^= 1;
			return res;
		}
		if (negative)
		{
			bigint dividend(*this);
			dividend.negative = false;
			auto res = dividend.divide(divisor);
			res.first.negative ^= 1;
			if (!res.second.isZero())
			{
				res.first -= 1u;
				res.second = divisor - res.second;
			}
			return res;
		}
		std::pair<bigint, bigint> res{ 0u, *this };
		if (!divisor.isZero())
		{
			// FIX ME
			while (res.second >= divisor)
			{
				++res.first;
				res.second -= divisor;
			}
		}
		return res;
	}

	void bigint::operator/=(const bigint& b)
	{
		*this = divide(b).first;
	}

	void bigint::operator%=(const bigint& b)
	{
		*this = divide(b).second;
	}

	void bigint::operator<<=(size_t b)
	{
		for (size_t i = getNumBits(); i-- != b; )
		{
			setBit(i, getBit(i - b));
		}
		for (size_t i = 0; i != b; ++i)
		{
			disableBit(i);
		}
	}

	void bigint::operator>>=(size_t b)
	{
		size_t bits = getNumBits();
		for (size_t i = 0; i++ != b; )
		{
			disableBit(bits - i);
		}
		shrink();
		bits -= b;
		for (size_t i = 0; i != bits; ++i)
		{
			setBit(i, getBit(i + b));
		}
	}

	bigint bigint::operator+(const bigint& b) const
	{
		bigint res(*this);
		res += b;
		return res;
	}

	void bigint::operator++()
	{
		*this += 1u;
	}

	bigint bigint::operator++(int)
	{
		bigint pre(*this);
		++*this;
		return pre;
	}

	bigint bigint::operator-(const bigint& subtrahend) const
	{
		bigint res(*this);
		res -= subtrahend;
		return res;
	}

	void bigint::operator--()
	{
		*this -= 1u;
	}

	bigint bigint::operator--(int)
	{
		bigint pre(*this);
		--*this;
		return pre;
	}

	bigint bigint::operator*(const bigint& b) const
	{
		bigint res(*this);
		res *= b;
		return res;
	}

	bigint bigint::operator/(const bigint& b) const
	{
		return divide(b).first;
	}

	bigint bigint::operator%(const bigint& b) const
	{
		return divide(b).second;
	}

	bigint bigint::operator<<(size_t b) const
	{
		bigint res(*this);
		res <<= b;
		return res;
	}

	bigint bigint::operator>>(size_t b) const
	{
		bigint res(*this);
		res >>= b;
		return res;
	}

	bool bigint::toPrimitive(size_t& out) const noexcept
	{
		switch (getNumChunks())
		{
		default:
			return false;

		case 1:
			out = getChunk(0);
			break;

		case 2:
			*(chunk_t*)&out = getChunk(0);
			*((chunk_t*)&out + 1) = getChunk(1);
		}
		return true;
	}

	std::string bigint::toStringHexUpper(bool prefix) const noexcept
	{
		return toStringHexImpl(prefix, string::from_int_map_hex_upper);
	}

	std::string bigint::toStringHexLower(bool prefix) const noexcept
	{
		return toStringHexImpl(prefix, string::from_int_map_hex_lower);
	}

	std::string bigint::toStringHexImpl(bool prefix, const char* map) const noexcept
	{
		size_t i = getNumNibbles();
		// skip leading zeroes
		while (i-- != 0 && getNibble(i) == 0);
		std::string str{};
		str.reserve(i + 1 + (prefix * 2) + negative);
		do
		{
			str.push_back(map[getNibble(i)]);
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
}
