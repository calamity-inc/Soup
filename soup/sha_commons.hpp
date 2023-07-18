#pragma once

namespace soup
{
	template <size_t BLOCK_INTS, bool intrin>
	void buffer_to_block(const std::string& buffer, uint32_t block[BLOCK_INTS])
	{
		/* Convert the std::string (byte buffer) to a uint32_t array (MSB) */
		for (size_t i = 0; i < BLOCK_INTS; i++)
		{
			if constexpr (intrin)
			{
				block[i] = (buffer[4 * i + 0] & 0xff)
					| (buffer[4 * i + 1] & 0xff) << 8
					| (buffer[4 * i + 2] & 0xff) << 16
					| (buffer[4 * i + 3] & 0xff) << 24;
			}
			else
			{
				block[i] = (buffer[4 * i + 3] & 0xff)
					| (buffer[4 * i + 2] & 0xff) << 8
					| (buffer[4 * i + 1] & 0xff) << 16
					| (buffer[4 * i + 0] & 0xff) << 24;
			}
		}
	}
}
