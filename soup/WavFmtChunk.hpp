#pragma once

#include "Packet.hpp"

namespace soup
{
	SOUP_PACKET(WavFmtChunk)
	{
		u16 format_tag;
		u16 channels;
		u32 sample_rate; // samples per second
		u32 avg_bytes_per_sec; // = (sample_rate * bytes_per_block)
		u16 bytes_per_block; // = ((bits_per_sample / 8) * channels)
		u16 bits_per_sample;

		SOUP_PACKET_IO(s)
		{
			return s.u16(format_tag)
				&& s.u16(channels)
				&& s.u32(sample_rate)
				&& s.u32(avg_bytes_per_sec)
				&& s.u16(bytes_per_block)
				&& s.u16(bits_per_sample)
				;
		}

		[[nodiscard]] bool isGoodForAudPlayback() const noexcept
		{
			return format_tag == 1 // WAVE_FORMAT_PCM
				&& (channels == 2 || channels == 1)
				&& sample_rate == 44100
				&& bits_per_sample == 16
				;
		}
	};
}
