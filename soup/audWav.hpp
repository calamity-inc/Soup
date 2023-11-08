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
		size_t data_end;

		audWav(ioSeekableReader& r);

		[[nodiscard]] bool hasFinished() noexcept final;
		double getAmplitude() final;
	};
}

#endif
