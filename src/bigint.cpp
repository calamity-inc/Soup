#include "bigint.hpp"

#include "rand.hpp"
#include "string.hpp"

namespace soup
{
	using chunk_t = bigint::chunk_t;
	using chunk_signed_t = bigint::chunk_signed_t;

	using namespace literals;

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
					res.fromBinary(str + 2, len - 2);
				}
				else if (str[1] == 'x' || str[1] == 'X')
				{
					res.fromHexadecimal(str + 2, len - 2);
				}
				else
				{
					res.fromDecimal(str, len);
				}
			}
			else
			{
				res.fromDecimal(str, len);
			}
			res.negative = neg;
		}
		return res;
	}

	void bigint::fromBinary(const char* str, size_t len)
	{
		for (size_t i = 0; i != len; ++i)
		{
			if (str[i] != '0')
			{
				enableBit(len - 1 - i);
			}
		}
	}

	void bigint::fromDecimal(const char* str, size_t len)
	{
		for (size_t i = 0; i != len; ++i)
		{
			*this *= 10u;
			*this += (unsigned int)(str[i] - '0');
		}
	}

	void bigint::fromHexadecimal(const char* str, size_t len)
	{
		for (size_t i = 0; i != len; ++i)
		{
			*this <<= 4u;
			if (str[i] >= 'a')
			{
				*this |= (unsigned int)(str[i] - ('a' - 10u));
			}
			else if (str[i] >= 'A')
			{
				*this |= (unsigned int)(str[i] - ('A' - 10u));
			}
			else
			{
				*this |= (unsigned int)(str[i] - '0');
			}
		}
	}

	bigint bigint::random(const size_t bits)
	{
		bigint res{};
		for (size_t i = 0; i != bits; ++i)
		{
			if (rand.coinflip())
			{
				res.enableBit(i);
			}
		}
		return res;
	}

	bigint bigint::randomProbablePrime(const size_t bits)
	{
		soup::bigint i = random(bits);
		for (; i.enableBit(0), !i.isProbablePrime(); i = random(bits));
		return i;
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
		if (i < chunks.size())
		{
			chunks[i] = v;
		}
		else
		{
			addChunk(v);
		}
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
			addChunk(chunk_i, 1 << (i % getBitsPerChunk()));
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

	size_t bigint::getBitLength() const noexcept
	{
		size_t len = getNumBits();
		while (len-- != 0 && !getBit(len));
		return len + 1;
	}

	size_t bigint::getLowestSetBit() const noexcept
	{
		for (size_t i = 0; i != getNumBits(); ++i)
		{
			if (getBit(i))
			{
				return i;
			}
		}
		return -1;
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
			return getNumChunks() > b.getNumChunks() ? +1 : -1;
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

	int bigint::cmpUnsigned(const bigint& b) const noexcept
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
		*this = (*this * b);
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
		std::pair<bigint, bigint> res{};
		if (!divisor.isZero())
		{
			for (size_t i = getNumBits(); i-- != 0; )
			{
				res.second <<= 1u;
				res.second.setBit(0, getBit(i));
				if (res.second >= divisor)
				{
					res.second -= divisor;
					res.first.enableBit(i);
				}
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

	bool bigint::isDivisorOf(const bigint& dividend) const
	{
		return (dividend % *this).isZero();
	}

	void bigint::operator<<=(size_t b)
	{
		size_t i = getNumBits();
		if (i != 0)
		{
			for (i += b; i != 0; --i)
			{
				setBit(i, getBit(i - b));
			}
			for (size_t i = 0; i != b; ++i)
			{
				disableBit(i);
			}
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

	void bigint::operator|=(const bigint& b)
	{
		for (size_t i = 0; i != b.getNumBits(); ++i)
		{
			if (b.getBit(i))
			{
				enableBit(i);
			}
		}
	}

	void bigint::operator&=(const bigint& b)
	{
		for (size_t i = 0; i != getNumBits(); ++i)
		{
			if (getBit(i) && !b.getBit(i))
			{
				disableBit(i);
			}
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
		*this += 1u;
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
		*this -= 1u;
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
		if (isZero())
		{
			return bigint();
		}
		if (*this < b)
		{
			return b * *this;
		}
		if (b.isZero())
		{
			return bigint();
		}
		bigint product{};
		product.negative = (negative ^ b.negative);
		for (size_t j = 0; j != b.getNumChunks(); ++j)
		{
			chunk_t carry = 0;
			const size_t y = b.getChunk(j);
			for (size_t i = 0; i != getNumChunks(); ++i)
			{
				const size_t x = getChunk(i);
				size_t res = product.getChunk(i + j) + (x * y) + carry;
				product.setChunk(i + j, (chunk_t)res);
				carry = getCarry(res);
			}
			if (carry != 0)
			{
				product.setChunk(j + getNumChunks(), carry);
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

	bigint bigint::abs() const
	{
		bigint res(*this);
		res.negative = false;
		return res;
	}

	bigint bigint::pow(bigint e) const
	{
		bigint res = 1u;
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

	bigint bigint::pow_mod(bigint e, const bigint& m) const
	{
		bigint res = 1u;
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
			base %= m;
		}
		res %= m;
		return res;
	}

	size_t bigint::getTrailingZeroes(const bigint& base) const
	{
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

	bigint bigint::gcd(bigint v) const
	{
		bigint u(*this);

		auto i = u.getTrailingZeroes(2u); u >>= i;
		auto j = v.getTrailingZeroes(2u); v >>= j;
		auto k = std::min(i, j);

		while (true)
		{
			if (u > v)
			{
				std::swap(u, v);
			}

			v -= u;

			if (v.isZero())
			{
				return u << k;
			}

			v >>= v.getTrailingZeroes(2u);
		}
	}

	bigint bigint::gcd(bigint b, bigint& x, bigint& y) const
	{
		if (isZero())
		{
			//x.reset();
			y = 1u;
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
		if (isZero() || *this == (chunk_t)1u)
		{
			ret = false;
			return true;
		}
		if (*this <= (chunk_t)3u)
		{
			ret = true;
			return true;
		}
		if (getBit(0) == 0)
		{
			ret = false;
			return true;
		}
		
		if ((*this % 3_b).isZero())
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

		for (bigint i = 5u; i * i <= *this; i += 6_b)
		{
			if ((*this % i).isZero() || (*this % (i + 2_b)).isZero())
			{
				return false;
			}
		}
		return true;
	}

	bool bigint::isProbablePrime(const int iterations) const
	{
		bool preret;
		if (isPrimePrecheck(preret))
		{
			return preret;
		}

		auto thisMinusOne = (*this - 1_b);
		auto a = thisMinusOne.getLowestSetBit();
		auto m = (thisMinusOne >> a);

		for (int i = 0; i < iterations; i++)
		{
			bigint b;
			do
			{
				b = random(getBitLength());
			} while (b <= (chunk_t)1u || b >= *this);

			int j = 0;
			bigint z = b.pow_mod(m, *this);
			while (!((j == 0 && z == (chunk_t)1u) || z == thisMinusOne))
			{
				if ((j > 0 && z == (chunk_t)1u) || ++j == a)
				{
					return false;
				}
				z = z.pow_mod(2u, *this);
			}
		}
		return true;
	}

	bool bigint::isCoprime(const bigint& b) const
	{
		return gcd(b) == (chunk_t)1u;
	}

	bigint bigint::reducedTotient() const
	{
		if (*this <= (chunk_t)2u)
		{
			return 1u;
		}
		std::vector<bigint> coprimes{};
		for (bigint a = 2u; a != *this; ++a)
		{
			if (isCoprime(a))
			{
				coprimes.emplace_back(a);
			}
		}
		bigint k = 2u;
		for (auto timer = coprimes.size(); timer != 0; )
		{
			for (auto i = coprimes.begin(); i != coprimes.end(); ++i)
			{
				if (i->pow_mod(k, *this) == (chunk_t)1u)
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

	bigint bigint::modMulInv(const bigint& m) const
	{
		bigint x, y;
		if (gcd(m, x, y) == (chunk_t)1u)
		{
			return (x % m + m) % m;
		}

		for (bigint res = 1u;; ++res)
		{
			if (((*this * res) % m) == (chunk_t)1u)
			{
				return res;
			}
		}
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
		os << v.toStringDecimal();
		return os;
	}
}
