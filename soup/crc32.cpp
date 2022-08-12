#include "crc32.hpp"

#include <array>

#include "Reader.hpp"

namespace soup
{
	[[nodiscard]] static std::array<uint32_t, 256> generate_crc_lookup_table() noexcept
	{
		constexpr uint32_t reversed_polynomial = 0xEDB88320ul;

		auto table = std::array<uint32_t, 256>{};
		for (uint32_t i = 0; i != table.size(); ++i)
		{
			uint32_t checksum = i;
			for (auto j = 0; j != 8; ++j)
			{
				checksum = (checksum >> 1) ^ ((checksum & 0x1u) * reversed_polynomial);
			}
			table[i] = checksum;
		}
		return table;
	}

	[[nodiscard]] const std::array<uint32_t, 256>& get_crc_lookup_table() noexcept
	{
		static const auto table = generate_crc_lookup_table();
		return table;
	}

	uint32_t crc32::hash(Reader& r)
	{
		const auto& table = get_crc_lookup_table();

		uint32_t checksum = 0xFFFFFFFFul;
		for (uint8_t c; r.u8(c); )
		{
			const uint32_t lookupIndex = (checksum ^ c) & 0xff;
			checksum = (checksum >> 8) ^ table[lookupIndex];
		}
		checksum ^= 0xFFFFFFFFu;
		return checksum;
	}

	uint32_t crc32::hash(const std::string& data)
	{
		const auto& table = get_crc_lookup_table();

		uint32_t checksum = 0xFFFFFFFFul;
		for (const auto& c : data)
		{
			const uint32_t lookupIndex = (checksum ^ (uint8_t)c) & 0xff;
			checksum = (checksum >> 8) ^ table[lookupIndex];
		}
		checksum ^= 0xFFFFFFFFu;
		return checksum;
	}

	uint32_t crc32::hash(const uint8_t* data, size_t size, uint32_t init)
	{
		const auto& table = get_crc_lookup_table();

		uint32_t checksum = ~init;
		for (; size != 0; ++data, --size)
		{
			const uint32_t lookupIndex = (checksum ^ *data) & 0xff;
			checksum = (checksum >> 8) ^ table[lookupIndex];
		}
		checksum ^= 0xFFFFFFFFu;
		return checksum;
	}
}
