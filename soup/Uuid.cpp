#include "Uuid.hpp"

#include "rand.hpp"
#include "string.hpp"

namespace soup
{
	Uuid Uuid::v4()
	{
		Uuid uuid;

		for (auto& i : uuid.ints)
		{
			i = soup::rand.t<uint32_t>(0, -1);
		}

		// Version 4
		uuid.bytes[7] &= 0x0f;
		uuid.bytes[7] |= 0x40;

		// Variant 1 (0b10)
		uuid.bytes[9] &= 0x3f;
		uuid.bytes[9] |= 0x80;

		return uuid;
	}

	uint8_t Uuid::getVersion() const
	{
		return bytes[7] >> 4;
	}

	std::string Uuid::toString() const
	{
		std::string str;
		str += string::lpad(string::hexLower(words[0]), 4, '0');
		str += string::lpad(string::hexLower(words[1]), 4, '0');
		str += '-';
		str += string::lpad(string::hexLower(words[2]), 4, '0');
		str += '-';
		str += string::lpad(string::hexLower(words[3]), 4, '0');
		str += '-';
		str += string::lpad(string::hexLower(words[5]), 4, '0');
		str += '-';
		str += string::lpad(string::hexLower(words[6]), 4, '0');
		str += string::lpad(string::hexLower(words[7]), 4, '0');
		return str;
	}
}
