#pragma once

#include <cstdint>
#include <cstring> // memcpy
#include <string>

namespace soup
{
	struct MacAddr
	{
		uint8_t data[6];

		static MacAddr DUMMY; // 00-1A-7D-DA-71-15: A very common MAC address from Bluetooth dongles that is unlikely to be used by other Ethernet users.

		MacAddr() = default;

		MacAddr(const uint8_t* data) noexcept
		{
			memcpy(this->data, data, 6);
		}

		MacAddr(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f) noexcept
			: data{ a, b, c, d, e, f }
		{
		}

		[[nodiscard]] std::string toString(char sep = '-') const;

		template <typename T>
		bool io(T& s)
		{
			return s.u8(data[0])
				&& s.u8(data[1])
				&& s.u8(data[2])
				&& s.u8(data[3])
				&& s.u8(data[4])
				&& s.u8(data[5])
				;
		}
	};
}
