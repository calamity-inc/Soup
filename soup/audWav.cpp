#include "audWav.hpp"

#if SOUP_WINDOWS

#include <iostream>

#include "audDevice.hpp" // AUD_MAX_SAMPLE
#include "Exception.hpp"
#include "riff.hpp"
#include "WavFmtChunk.hpp"

namespace soup
{
	audWav::audWav(ioSeekableReader& r)
		: r(r)
	{
		RiffReader rr(r);
		SOUP_IF_UNLIKELY (!rr.seekChunk("fmt ").isValid())
		{
			SOUP_THROW(Exception("WAV file seems to be in an invalid format"));
		}
		WavFmtChunk fmt;
		fmt.read(r);
		SOUP_IF_UNLIKELY (!fmt.isGoodForAudPlayback())
		{
			SOUP_IF_UNLIKELY (fmt.channels != 2 && fmt.channels != 1)
			{
				SOUP_THROW(Exception("WAV file must be mono or stereo (such that 1 <= channels <= 2 holds true)"));
			}
			SOUP_THROW(Exception("WAV file must be 16-bit (L)PCM at 44100 samples per second"));
		}
		channels = fmt.channels;
		auto ck = rr.seekChunk("data");
		data_begin = ck.data_offset;
		data_end = data_begin + ck.data_size;
		SOUP_IF_UNLIKELY (!ck.isValid())
		{
			SOUP_THROW(Exception("WAV file seems to be in an invalid format"));
		}
		//duration_seconds = (double)ck.data_size / fmt.avg_bytes_per_sec;
	}

	bool audWav::hasFinished() noexcept
	{
		return r.getPosition() == data_end;
	}

	double audWav::getAmplitude()
	{
		// Handle looping
		SOUP_IF_UNLIKELY (hasFinished())
		{
			r.seek(data_begin);
		}

		int16_t sample;
		r.i16(sample);
		return (double)sample / AUD_MAX_SAMPLE;
	}
}

#endif
