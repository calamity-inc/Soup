#pragma once

#include "base.hpp"

#if SOUP_WINDOWS

#include "audSound.hpp"
#include "ioSeekableReader.hpp"

namespace soup
{
	struct audWav : public audSound
	{
		ioSeekableReader& r;
		uint16_t channels;
		size_t data_begin;
		double duration_seconds;

		audWav(ioSeekableReader& r);

		double getAmplitude(double t) final;

		double getDurationSeconds() noexcept final
		{
			return duration_seconds;
		}
	};
}

#endif
