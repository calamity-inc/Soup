#include "Uuid.hpp"

#include "Endian.hpp"
#include "rand.hpp"
#include "string.hpp"

NAMESPACE_SOUP
{
	Uuid Uuid::v4()
	{
		Uuid uuid;

		for (auto& i : uuid.be_ints)
		{
			i = soup::rand.t<uint32_t>(0, -1);
		}

		// Version 4
		uuid.bytes[6] &= 0x0f;
		uuid.bytes[6] |= 0x40;

		// Variant 1 (0b10)
		uuid.bytes[8] &= 0x3f;
		uuid.bytes[8] |= 0x80;

		return uuid;
	}

	uint8_t Uuid::getVersion() const
	{
		return bytes[7] >> 4;
	}

	std::string Uuid::toString() const
	{
		std::string str;
		str += string::lpad(string::hexLower(Endianness::toNative((network_u16_t)be_words[0])), 4, '0');
		str += string::lpad(string::hexLower(Endianness::toNative((network_u16_t)be_words[1])), 4, '0');
		str += '-';
		str += string::lpad(string::hexLower(Endianness::toNative((network_u16_t)be_words[2])), 4, '0');
		str += '-';
		str += string::lpad(string::hexLower(Endianness::toNative((network_u16_t)be_words[3])), 4, '0');
		str += '-';
		str += string::lpad(string::hexLower(Endianness::toNative((network_u16_t)be_words[4])), 4, '0');
		str += '-';
		str += string::lpad(string::hexLower(Endianness::toNative((network_u16_t)be_words[5])), 4, '0');
		str += string::lpad(string::hexLower(Endianness::toNative((network_u16_t)be_words[6])), 4, '0');
		str += string::lpad(string::hexLower(Endianness::toNative((network_u16_t)be_words[7])), 4, '0');
		return str;
	}
}
