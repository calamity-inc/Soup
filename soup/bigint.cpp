#include "bigint.hpp"

#include "bitset.hpp"
#include "endianness.hpp"
#include "optimised.hpp"
#include "rand.hpp"
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

	bigint bigint::fromString(const char* str, size_t len)
	{
		bigint res{};
		if (len != 0)
		{
			const bool neg = (str[0] == '-');
			if (neg)
			{
				++str;
				--len;
			}
			if (len > 2 && str[0] == '0')
			{
				if (str[1] == 'b' || str[1] == 'B')
				{
					res.fromStringImplBinary(str + 2, len - 2);
				}
				else if (str[1] == 'x' || str[1] == 'X')
				{
					res.fromStringImplHex(str + 2, len - 2);
				}
				else
				{
					res.fromStringImplDecimal(str, len);
				}
			}
			else
			{
				res.fromStringImplDecimal(str, len);
			}
			res.negative = neg;
		}
		return res;
	}

	bigint bigint::fromStringHex(const char* str, size_t len)
	{
		bigint res{};
		if (len != 0)
		{
			const bool neg = (str[0] == '-');
			if (neg)
			{
				++str;
				--len;
			}
			res.fromStringImplHex(str, len);
			res.negative = neg;
		}
		return res;
	}

	void bigint::fromStringImplBinary(const char* str, size_t len)
	{
		for (size_t i = 0; i != len; ++i)
		{
			if (str[i] != '0')
			{
				enableBit(len - 1 - i);
			}
		}
	}

	void bigint::fromStringImplDecimal(const char* str, size_t len)
	{
		for (size_t i = 0; i != len; ++i)
		{
			*this *= bigint((chunk_t)10u);
			*this += (chunk_t)(str[i] - '0');
		}
	}

	void bigint::fromStringImplHex(const char* str, size_t len)
	{
		for (size_t i = 0; i != len; ++i)
		{
			*this <<= 4u;
			if (str[i] >= 'a')
			{
				*this |= (chunk_t)(str[i] - ('a' - 10u));
			}
			else if (str[i] >= 'A')
			{
				*this |= (chunk_t)(str[i] - ('A' - 10u));
			}
			else
			{
				*this |= (chunk_t)(str[i] - '0');
			}
		}
	}

	bigint bigint::random(size_t bits)
	{
		bigint res{};
		if ((bits % getBitsPerChunk()) == 0)
		{
			bits /= getBitsPerChunk();
			for (size_t i = 0; i != bits; ++i)
			{
				res.chunks.emplace_back(rand.t<chunk_t>(0, -1));
			}
		}
		else
		{
			for (size_t i = 0; i != bits; ++i)
			{
				if (rand.coinflip())
				{
					res.enableBit(i);
				}
			}
		}
		return res;
	}

	bigint bigint::randomProbablePrime(const size_t bits, const int miller_rabin_iterations)
	{
		bigint i = random(bits);
		for (; i.enableBitInbounds(0), !i.isProbablePrime(miller_rabin_iterations); i = random(bits));
		return i;
	}

	chunk_t bigint::getChunk(size_t i) const noexcept
	{
		if (i < getNumChunks())
		{
			return getChunkInbounds(i);
		}
		return 0;
	}

	chunk_t bigint::getChunkInbounds(size_t i) const noexcept
	{
		return chunks[i];
	}

	void bigint::setChunk(size_t i, chunk_t v)
	{
		if (i < chunks.size())
		{
			setChunkInbounds(i, v);
		}
		else
		{
			addChunk(v);
		}
	}

	void bigint::setChunkInbounds(size_t i, chunk_t v)
	{
		chunks[i] = v;
	}

	void bigint::addChunk(size_t i, chunk_t v)
	{
		while (i != chunks.size())
		{
			addChunk(0);
		}
		addChunk(v);
	}

	void bigint::addChunk(chunk_t v)
	{
		chunks.emplace_back(v);
	}

	void bigint::shrink() noexcept
	{
		for (size_t i = chunks.size(); i-- != 0; )
		{
			if (chunks[i] != 0)
			{
				break;
			}
			chunks.erase(i);
		}
	}

	size_t bigint::getNumBytes() const noexcept
	{
		return getNumChunks() * getBytesPerChunk();
	}

	uint8_t bigint::getByte(const size_t i) const noexcept
	{
		auto j = i / getBytesPerChunk();
		auto k = i % getBytesPerChunk();

		if (j < chunks.size())
		{
			return (reinterpret_cast<const uint8_t*>(&chunks[j]))[k];
		}
		return 0;
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

	bool bigint::getBit(const size_t i) const noexcept
	{
		auto chunk_i = i / getBitsPerChunk();
		auto j = i % getBitsPerChunk();

		return (getChunk(chunk_i) >> j) & 1;
	}

	void bigint::setBit(const size_t i, const bool v)
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

	void bigint::enableBit(const size_t i)
	{
		auto chunk_i = i / getBitsPerChunk();
		auto j = i % getBitsPerChunk();

		j = (1 << j);

		if (chunk_i < chunks.size())
		{
			chunks[chunk_i] |= j;
		}
		else
		{
			addChunk(chunk_i, j);
		}
	}

	void bigint::disableBit(const size_t i)
	{
		auto chunk_i = i / getBitsPerChunk();
		auto j = i % getBitsPerChunk();

		if (chunk_i < chunks.size())
		{
			chunks[chunk_i] &= ~(1 << j);
		}
	}

	size_t bigint::getBitLength() const noexcept
	{
		size_t len = getNumBits();
		while (len-- != 0 && !getBit(len));
		return len + 1;
	}

	size_t bigint::getLowestSetBit() const noexcept
	{
		const auto nb = getNumBits();
		for (size_t i = 0; i != nb; ++i)
		{
			if (getBit(i))
			{
				return i;
			}
		}
		return -1;
	}

	bool bigint::getBitInbounds(const size_t i) const noexcept
	{
		auto chunk_i = i / getBitsPerChunk();
		auto j = i % getBitsPerChunk();

		return (getChunkInbounds(chunk_i) >> j) & 1;
	}

	void bigint::setBitInbounds(const size_t i, const bool v)
	{
		auto chunk_i = i / getBitsPerChunk();
		auto j = i % getBitsPerChunk();

		bitset<chunk_t>::at(chunks[chunk_i]).set(j, v);
	}

	void bigint::enableBitInbounds(const size_t i)
	{
		auto chunk_i = i / getBitsPerChunk();
		auto j = i % getBitsPerChunk();

		chunks[chunk_i] |= (1 << j);
	}

	void bigint::disableBitInbounds(const size_t i)
	{
		auto chunk_i = i / getBitsPerChunk();
		auto j = i % getBitsPerChunk();

		chunks[chunk_i] &= ~(1 << j);
	}

	void bigint::reset() noexcept
	{
		chunks.clear();
		negative = false;
	}

	bool bigint::isZero() const noexcept
	{
		return getNumChunks() == 0;
	}

	bigint::operator bool() const noexcept
	{
		return !isZero();
	}

	int bigint::cmp(const bigint& b) const noexcept
	{
		if (getNumChunks() != b.getNumChunks())
		{
			return optimised::trinary(getNumChunks() > b.getNumChunks(), +1, -1);
		}
		if (negative)
		{
			if (!b.negative)
			{
				return -1;
			}
		}
		else
		{
			if (b.negative)
			{
				return +1;
			}
		}
		size_t i = chunks.size();
		while (i != 0)
		{
			--i;
			if (getChunkInbounds(i) != b.getChunkInbounds(i))
			{
				return optimised::trinary(getChunkInbounds(i) > b.getChunkInbounds(i), +1, -1);
			}
		}
		return 0;
	}

	int bigint::cmpUnsigned(const bigint& b) const noexcept
	{
		if (getNumChunks() != b.getNumChunks())
		{
			return optimised::trinary(getNumChunks() > b.getNumChunks(), +1, -1);
		}
		size_t i = chunks.size();
		while (i != 0)
		{
			--i;
			if (getChunk(i) != b.getChunk(i))
			{
				return optimised::trinary(getChunk(i) > b.getChunk(i), +1, -1);
			}
		}
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

	bool bigint::operator==(const chunk_t v) const noexcept
	{
		return !negative && getNumChunks() == 1 && getChunk(0) == v;
	}

	bool bigint::operator!=(const chunk_t v) const noexcept
	{
		return !operator==(v);
	}

	bool bigint::operator<(const chunk_t v) const noexcept
	{
		return negative || (getNumChunks() == 0 && v != 0) || (getNumChunks() == 1 && getChunk(0) < v);
	}

	bool bigint::operator<=(const chunk_t v) const noexcept
	{
		return negative || getNumChunks() == 0 || (getNumChunks() == 1 && getChunk(0) <= v);
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
		chunks.clear();
		if (v != 0)
		{
			chunks.emplace_back(v);
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
#if SOUP_WINDOWS
			chunks.clear();
			chunks.emplace_back((chunk_t)v);
			chunks.emplace_back(carry);
#else
			chunks = std::vector<chunk_t>{ (chunk_t)v, carry };
#endif
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
		if (negative ^ b.negative)
		{
			subUnsigned(b);
		}
		else
		{
			addUnsigned(b);
		}
	}

	void bigint::addUnsigned(const bigint& b)
	{
		if (cmp(b) < 0)
		{
			bigint res(b);
			res.addUnsigned(*this);
			chunks = std::move(res.chunks);
			return;
		}
		chunk_t carry = 0;
		const size_t j = getNumChunks();
		for (size_t i = 0; i != j; ++i)
		{
			const size_t x = getChunkInbounds(i);
			const size_t y = b.getChunk(i);
			size_t res = (x + y + carry);
			setChunkInbounds(i, (chunk_t)res);
			carry = getCarry(res);
		}
		if (carry != 0)
		{
			addChunk(carry);
		}
	}

	void bigint::operator-=(const bigint& subtrahend)
	{
		if (negative ^ subtrahend.negative)
		{
			addUnsigned(subtrahend);
		}
		else
		{
			subUnsigned(subtrahend);
		}
	}

	void bigint::subUnsigned(const bigint& subtrahend)
	{
		if (cmpUnsigned(subtrahend) == 0)
		{
			reset();
			return;
		}
		if (cmp(subtrahend) < 0)
		{
			bigint res(subtrahend);
			res.subUnsigned(*this);
			chunks = std::move(res.chunks);
			negative ^= 1;
			return;
		}
		size_t carry = 0;
		const size_t j = getNumChunks();
		for (size_t i = 0; i != j; ++i)
		{
			const size_t x = getChunkInbounds(i);
			const size_t y = subtrahend.getChunk(i);
			size_t res = (x - y - carry);
			setChunkInbounds(i, (chunk_t)res);
			carry = (getCarry(res) != 0);
		}
		shrink();
	}

	void bigint::operator*=(const bigint& b)
	{
		*this = (*this * b);
	}

	void bigint::operator/=(const bigint& divisor)
	{
		*this = divide(divisor).first;
	}

	void bigint::operator%=(const bigint& divisor)
	{
		if (*this >= divisor)
		{
			*this = divide(divisor).second;
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
				res.first -= bigint((chunk_t)1u);
				res.second = divisor - res.second;
			}
			return res;
		}
		return divideUnsigned(divisor);
	}

	std::pair<bigint, bigint> bigint::divideUnsigned(const bigint& divisor) const
	{
		std::pair<bigint, bigint> res{};
		if (!divisor.isZero())
		{
			if (divisor == bigint((chunk_t)2u))
			{
				res.first = *this;
				res.first >>= 1u;
				res.second = (chunk_t)getBit(0);
			}
			else
			{
				for (size_t i = getNumBits(); i-- != 0; )
				{
					res.second.leftShiftSmall(1);
					res.second.setBit(0, getBitInbounds(i));
					if (res.second >= divisor)
					{
						res.second -= divisor;
						res.first.enableBit(i);
					}
				}
			}
		}
		return res;
	}

	bigint bigint::mod(const bigint& m) const
	{
		return divide(m).second;
	}

	bigint bigint::modUnsigned(const bigint& m) const
	{
		auto m_minus_1 = (m - bigint((chunk_t)1u));
		if ((m & m_minus_1).isZero())
		{
			return (*this & m_minus_1);
		}
		return modUnsignedNotpowerof2(m);
	}

	bigint bigint::modUnsignedPowerof2(const bigint& m) const
	{
		return (*this & (m - bigint((chunk_t)1u)));
	}

	bigint bigint::modUnsignedNotpowerof2(const bigint& divisor) const
	{
		bigint remainder{};
		for (size_t i = getNumBits(); i-- != 0; )
		{
			remainder.leftShiftSmall(1);
			remainder.setBit(0, getBitInbounds(i));
			if (remainder >= divisor)
			{
				remainder.subUnsigned(divisor);
			}
		}
		return remainder;
	}

	bool bigint::isDivisorOf(const bigint& dividend) const
	{
		return (dividend % *this).isZero();
	}

	void bigint::operator<<=(const size_t b)
	{
		if (b <= getBitsPerChunk())
		{
			return leftShiftSmall(b);
		}

		const auto nb = getNumBits();
		if (nb != 0)
		{
			const auto maxbitidx = nb - 1;
			// extend
			{
				size_t i = maxbitidx;
				for (size_t j = 0; j != b; ++j)
				{
					setBit(i + b, getBitInbounds(i));
					if (i-- == 0)
					{
						break;
					}
				}
			}

			// move
			if (b < nb)
			{
				for (size_t i = nb; i-- != b; )
				{
					setBitInbounds(i, getBitInbounds(i - b));
				}
			}

			// disable
			if (b < getNumBits())
			{
				for (size_t i = 0; i != b; ++i)
				{
					disableBitInbounds(i);
				}
			}
		}
	}

	void bigint::leftShiftSmall(const size_t b)
	{
		size_t carry = 0;
		const auto nc = getNumChunks();
		for (size_t i = 0; i != nc; ++i)
		{
			if constexpr (SOUP_LITTLE_ENDIAN)
			{
				chunk_t c[2];
				c[0] = getChunkInbounds(i);
				c[1] = 0;
				*reinterpret_cast<size_t*>(&c[0]) = ((*reinterpret_cast<size_t*>(&c[0]) << b) | carry);
				setChunkInbounds(i, c[0]);
				carry = c[1];
			}
			else
			{
				size_t c = getChunkInbounds(i);
				c = ((c << b) | carry);
				setChunkInbounds(i, c);
				carry = getCarry(c);
			}
		}
		if (carry != 0)
		{
			addChunk(carry);
		}
	}

	void bigint::operator>>=(const size_t b)
	{
		if constexpr (SOUP_LITTLE_ENDIAN)
		{
			if (b <= getBitsPerChunk())
			{
				const auto nc = getNumChunks();
				if (nc != 0)
				{
					chunk_t carry = 0;
					for (size_t i = nc; i-- != 0; )
					{
						chunk_t c[2];
						c[0] = 0;
						c[1] = getChunkInbounds(i);
						*reinterpret_cast<size_t*>(&c[0]) = (*reinterpret_cast<size_t*>(&c[0]) >> b);
						setChunkInbounds(i, c[1] | carry);
						carry = c[0];
					}
					shrink();
				}
				return;
			}
		}

		size_t nb = getNumBits();
		if (nb != 0)
		{
			for (size_t i = 0; i != nb; ++i)
			{
				setBitInbounds(i, getBit(i + b));
			}
			for (size_t i = nb, j = 0; --i, j != b; ++j)
			{
				disableBitInbounds(i);
			}
			shrink();
		}
	}

	void bigint::operator|=(const bigint& b)
	{
		const auto nb = b.getNumBits();
		for (size_t i = 0; i != nb; ++i)
		{
			if (b.getBit(i))
			{
				enableBit(i);
			}
		}
	}

	void bigint::operator&=(const bigint& b)
	{
		const auto nc = getNumChunks();
		for (size_t i = 0; i != nc; ++i)
		{
			setChunkInbounds(i, getChunkInbounds(i) & b.getChunk(i));
		}
		shrink();
	}

	bigint bigint::operator+(const bigint& b) const
	{
		bigint res(*this);
		res += b;
		return res;
	}

	bigint& bigint::operator++()
	{
		*this += bigint((chunk_t)1u);
		return *this;
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

	bigint& bigint::operator--()
	{
		*this -= bigint((chunk_t)1u);
		return *this;
	}

	bigint bigint::operator--(int)
	{
		bigint pre(*this);
		--*this;
		return pre;
	}

	bigint bigint::operator*(const bigint& b) const
	{
		bigint product{};
		if (!isZero() && !b.isZero())
		{
			product.negative = (negative ^ b.negative);
			const auto nc = getNumChunks();
			const auto b_nc = b.getNumChunks();
			for (size_t j = 0; j != b_nc; ++j)
			{
				chunk_t carry = 0;
				const size_t y = b.getChunkInbounds(j);
				for (size_t i = 0; i != nc; ++i)
				{
					const size_t x = getChunkInbounds(i);
					size_t res = product.getChunk(i + j) + (x * y) + carry;
					product.setChunk(i + j, (chunk_t)res);
					carry = getCarry(res);
				}
				if (carry != 0)
				{
					product.setChunk(j + nc, carry);
				}
			}
		}
		return product;
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

	bigint bigint::operator|(const bigint& b) const
	{
		bigint res(*this);
		res |= b;
		return res;
	}

	bigint bigint::operator&(const bigint& b) const
	{
		bigint res(*this);
		res &= b;
		return res;
	}

	bool bigint::isEven() const noexcept
	{
		return !isOdd();
	}

	bool bigint::isOdd() const noexcept
	{
		return getBit(0);
	}

	bigint bigint::abs() const
	{
		bigint res(*this);
		res.negative = false;
		return res;
	}

	bigint bigint::pow(bigint e) const
	{
		if (*this == bigint((chunk_t)2u))
		{
			size_t e_primitive;
			if (e.toPrimitive(e_primitive))
			{
				return _2pow(e_primitive);
			}
		}
		return powNot2(e);
	}

	bigint bigint::powNot2(bigint e) const
	{
		bigint res = bigint((chunk_t)1u);
		bigint base(*this);
		while (true)
		{
			if (e.getBit(0))
			{
				res *= base;
			}
			e >>= 1u;
			if (e.isZero())
			{
				break;
			}
			base *= base;
		}
		return res;
	}

	bigint bigint::pow2() const
	{
		return *this * *this;
	}

	bigint bigint::_2pow(size_t e)
	{
		return bigint((chunk_t)1u) << e;
	}

	size_t bigint::getTrailingZeroes(const bigint& base) const
	{
		if (base == bigint((chunk_t)2u))
		{
			return getTrailingZeroesBinary();
		}
		size_t res = 0;
		bigint tmp(*this);
		while (!tmp.isZero())
		{
			auto pair = tmp.divide(base);
			if (!pair.second.isZero())
			{
				break;
			}
			++res;
			tmp = std::move(pair.first);
		}
		return res;
	}

	size_t bigint::getTrailingZeroesBinary() const
	{
		size_t res = 0;
		const auto nb = getNumBits();
		for (size_t i = 0; i != nb && !getBit(i); ++i)
		{
			++res;
		}
		return res;
	}

	bigint bigint::gcd(bigint v) const
	{
		bigint u(*this);

		auto i = u.getTrailingZeroesBinary(); u >>= i;
		auto j = v.getTrailingZeroesBinary(); v >>= j;
		auto k = optimised::min(i, j);

		while (true)
		{
			if (u > v)
			{
				std::swap(u, v);
			}

			v -= u;

			if (v.isZero())
			{
				u <<= k;
				return u;
			}

			v >>= v.getTrailingZeroesBinary();
		}
	}

	bigint bigint::gcd(bigint b, bigint& x, bigint& y) const
	{
		if (isZero())
		{
			//x.reset();
			y = bigint((chunk_t)1u);
			return b;
		}
		auto d = b.divide(*this);
		bigint xr, yr;
		auto g = d.second.gcd(*this, xr, yr);
		x = (yr - (d.first * xr));
		y = std::move(xr);
		return g;
	}

	bool bigint::isPrimePrecheck(bool& ret) const
	{
		if (isZero() || *this == bigint((chunk_t)1u))
		{
			ret = false;
			return true;
		}
		if (*this <= bigint((chunk_t)3u))
		{
			ret = true;
			return true;
		}
		if (getBit(0) == 0)
		{
			ret = false;
			return true;
		}
		
		if (this->modUnsigned(bigint((chunk_t)3u)).isZero())
		{
			ret = false;
			return true;
		}
		return false;
	}

	bool bigint::isPrime() const
	{
		bool preret;
		if (isPrimePrecheck(preret))
		{
			return preret;
		}

		if (getNumBits() < 128)
		{
			return isPrimeAccurateNoprecheck();
		}

		return isProbablePrimeNoprecheck(10);
	}

	bool bigint::isPrimeAccurate() const
	{
		bool preret;
		if (isPrimePrecheck(preret))
		{
			return preret;
		}

		return isPrimeAccurateNoprecheck();
	}

	bool bigint::isPrimeAccurateNoprecheck() const
	{
		for (bigint i = bigint((chunk_t)5u); i * i <= *this; i += bigint((chunk_t)6u))
		{
			if ((*this % i).isZero() || (*this % (i + bigint((chunk_t)2u))).isZero())
			{
				return false;
			}
		}
		return true;
	}

	bool bigint::isProbablePrime(const int miller_rabin_iterations) const
	{
		bool preret;
		if (isPrimePrecheck(preret))
		{
			return preret;
		}

		return isProbablePrimeNoprecheck(miller_rabin_iterations);
	}

	bool bigint::isProbablePrimeNoprecheck(const int miller_rabin_iterations) const
	{
		auto thisMinusOne = (*this - bigint((chunk_t)1u));
		auto a = thisMinusOne.getLowestSetBit();
		auto m = (thisMinusOne >> a);

		const auto bl = getBitLength();
		for (int i = 0; i != miller_rabin_iterations; i++)
		{
			bigint b;
			do
			{
				b = random(bl);
			} while (b >= *this || b <= bigint((chunk_t)1u));

			int j = 0;
			bigint z = b.modPow(m, *this);
			while (!((j == 0 && z == bigint((chunk_t)1u)) || z == thisMinusOne))
			{
				if ((j > 0 && z == bigint((chunk_t)1u)) || ++j == a)
				{
					return false;
				}
				// z = z.modPow(2u, *this);
				z = z.modMulUnsignedNotpowerof2(z, *this);
			}
		}
		return true;
	}

	bool bigint::isCoprime(const bigint& b) const
	{
		return gcd(b) == bigint((chunk_t)1u);
	}

	bigint bigint::eulersTotient() const
	{
		bigint res = bigint((chunk_t)1u);
		for (bigint i = bigint((chunk_t)2u); i != *this; ++i)
		{
			if (isCoprime(i))
			{
				++res;
			}
		}
		return res;
	}

	bigint bigint::reducedTotient() const
	{
		if (*this <= bigint((chunk_t)2u))
		{
			return bigint((chunk_t)1u);
		}
		std::vector<bigint> coprimes{};
		for (bigint a = bigint((chunk_t)2u); a != *this; ++a)
		{
			if (isCoprime(a))
			{
				coprimes.emplace_back(a);
			}
		}
		bigint k = bigint((chunk_t)2u);
		for (auto timer = coprimes.size(); timer != 0; )
		{
			for (auto i = coprimes.begin(); i != coprimes.end(); ++i)
			{
				if (i->modPow(k, *this) == bigint((chunk_t)1u))
				{
					if (timer == 0)
					{
						break;
					}
					--timer;
				}
				else
				{
					timer = coprimes.size();
					++k;
				}
			}
		}
		return k;
	}

	bigint bigint::lcm(const bigint& b) const
	{
		if (isZero() || b.isZero())
		{
			return bigint();
		}
		auto a_mag = abs();
		auto b_mag = b.abs();
		return ((a_mag * b_mag) / a_mag.gcd(b_mag));
	}

	bool bigint::isPowerOf2() const
	{
		return (*this & (*this - bigint((chunk_t)1u))).isZero();
	}

	bigint bigint::modMulInv(const bigint& m) const
	{
		bigint x, y;
		if (gcd(m, x, y) == bigint((chunk_t)1u))
		{
			return (x % m + m) % m;
		}

		for (bigint res = bigint((chunk_t)1u);; ++res)
		{
			if (((*this * res) % m) == bigint((chunk_t)1u))
			{
				return res;
			}
		}
	}

	void bigint::modMulInv2Coprimes(const bigint& a, const bigint& m, bigint& x, bigint& y)
	{
		a.gcd(m, x, y);
		x = ((x % m + m) % m);
		y = ((y % a + a) % a);
	}

	bigint bigint::modMulUnsigned(const bigint& b, const bigint& m) const
	{
		return (*this * b).modUnsigned(m);
	}

	bigint bigint::modMulUnsignedNotpowerof2(const bigint& b, const bigint& m) const
	{
		return (*this * b).modUnsignedNotpowerof2(m);
	}

	bigint bigint::modPow(bigint e, const bigint& m) const
	{
		if (m.isOdd()
			&& e.getNumBits() > 32 // arbitrary choice
			)
		{
			return modPowMontgomery(e, m);
		}
		return modPowBasic(e, m);
	}

	bigint bigint::modPowMontgomery(bigint e, const bigint& m) const
	{
		auto re = m.montgomeryREFromM();
		auto r = montgomeryRFromRE(re);
		bigint m_mod_mul_inv, r_mod_mul_inv;
		modMulInv2Coprimes(m, r, m_mod_mul_inv, r_mod_mul_inv);
		return modPowMontgomery(std::move(e), re, r, m, r_mod_mul_inv, m_mod_mul_inv, r.modUnsignedNotpowerof2(m));
	}

	bigint bigint::modPowMontgomery(bigint e, size_t re, const bigint& r, const bigint& m, const bigint& r_mod_mul_inv, const bigint& m_mod_mul_inv, const bigint& one_mont) const
	{
		bigint res = one_mont;
		bigint base = enterMontgomerySpace(r, m);
		/*bigint base(*this);
		if (base >= m)
		{
			base = base.modUnsigned(m);
		}
		base = base.enterMontgomerySpace(r, m);*/
		while (!e.isZero())
		{
			if (e.getBit(0))
			{
				res = res.montgomeryMultiplyEfficient(base, r, re, m, m_mod_mul_inv);
			}
			base = base.montgomeryMultiplyEfficient(base, r, re, m, m_mod_mul_inv);
			e >>= 1u;
		}
		return res.leaveMontgomerySpaceEfficient(r_mod_mul_inv, m);
	}

	bigint bigint::modPowBasic(bigint e, const bigint& m) const
	{
		bigint base(*this);
		if (base >= m)
		{
			base = base.modUnsigned(m);
		}
		bigint res = bigint((chunk_t)1u);
		while (!e.isZero())
		{
			if (e.getBit(0))
			{
				res = res.modMulUnsignedNotpowerof2(base, m);
			}
			base = base.modMulUnsignedNotpowerof2(base, m);
			e >>= 1u;
		}
		return res;
	}

	// We need a positive integer r such that r >= m && r.isCoprime(m)
	// We assume an odd modulus, so any power of 2 will be coprime to it.

	size_t bigint::montgomeryREFromM() const
	{
		return getBitLength();
	}

	bigint bigint::montgomeryRFromRE(size_t re)
	{
		return _2pow(re);
	}

	bigint bigint::montgomeryRFromM() const
	{
		return montgomeryRFromRE(montgomeryREFromM());
	}

	bigint bigint::enterMontgomerySpace(const bigint& r, const bigint& m) const
	{
		return modMulUnsignedNotpowerof2(r, m);
	}

	bigint bigint::leaveMontgomerySpace(const bigint& r, const bigint& m) const
	{
		return leaveMontgomerySpaceEfficient(r.modMulInv(m), m);
	}

	bigint bigint::leaveMontgomerySpaceEfficient(const bigint& r_mod_mul_inv, const bigint& m) const
	{
		return modMulUnsignedNotpowerof2(r_mod_mul_inv, m);
	}

	bigint bigint::montgomeryMultiply(const bigint& b, const bigint& r, const bigint& m) const
	{
		return (*this * b).montgomeryReduce(r, m);
	}

	bigint bigint::montgomeryMultiplyEfficient(const bigint& b, const bigint& r, size_t re, const bigint& m, const bigint& m_mod_mul_inv) const
	{
		return (*this * b).montgomeryReduce(r, re, m, m_mod_mul_inv);
	}

	bigint bigint::montgomeryReduce(const bigint& r, const bigint& m) const
	{
		return montgomeryReduce(r, m, m.modMulInv(r));
	}

	bigint bigint::montgomeryReduce(const bigint& r, const bigint& m, const bigint& m_mod_mul_inv) const
	{
		auto q = (modUnsignedPowerof2(r) * m_mod_mul_inv).modUnsignedPowerof2(r);
		auto a = (*this - q * m) / r;
		if (a.negative)
		{
			a += m;
		}
		return a;
	}

	bigint bigint::montgomeryReduce(const bigint& r, size_t re, const bigint& m, const bigint& m_mod_mul_inv) const
	{
		auto q = (modUnsignedPowerof2(r) * m_mod_mul_inv).modUnsignedPowerof2(r);
		auto a = (*this - q * m);
		a >>= re;
		if (a.negative)
		{
			a += m;
		}
		return a;
	}

	bool bigint::toPrimitive(size_t& out) const
	{
		switch (getNumChunks())
		{
		default:
			return false;

		case 0:
			out = 0;
			break;

		case 1:
			out = getChunk(0);
			break;

		case 2:
			*(chunk_t*)&out = getChunk(0);
			*((chunk_t*)&out + 1) = getChunk(1);
		}
		return true;
	}

	std::string bigint::toStringHex(bool prefix) const
	{
		return toStringHexImpl(prefix, string::charset_hex);
	}

	std::string bigint::toStringHexLower(bool prefix) const
	{
		return toStringHexImpl(prefix, string::charset_hex_lower);
	}

	std::string bigint::toStringHexImpl(bool prefix, const char* map) const
	{
		std::string str{};
		size_t i = getNumNibbles();
		if (i == 0)
		{
			str.push_back('0');
		}
		else
		{
			// skip leading zeroes
			while (i-- != 0 && getNibble(i) == 0);
			str.reserve(i + 1 + (prefix * 2) + negative);
			do
			{
				str.push_back(map[getNibble(i)]);
			} while (i-- != 0);
		}
		if (prefix)
		{
			str.insert(0, 1, 'x');
			str.insert(0, 1, '0');
		}
		if (negative)
		{
			str.insert(0, 1, '-');
		}
		return str;
	}

	std::ostream& operator<<(std::ostream& os, const bigint& v)
	{
		return os << v.toString();
	}

	bigint bigint::fromBinary(const std::string& msg)
	{
		bigint res{};
		for (auto i = msg.begin(); i != msg.end(); ++i)
		{
			res <<= 8u;
			res |= (chunk_t)(unsigned char)*i;
		}
		return res;
	}

	std::string bigint::toBinary() const
	{
		std::string str{};
		size_t i = getNumBytes();
		if (i != 0)
		{
			// skip leading zeroes
			while (i-- != 0 && getByte(i) == 0);
			str.reserve(i);
			do
			{
				str.push_back(getByte(i));
			} while (i-- != 0);
		}
		return str;
	}
}
