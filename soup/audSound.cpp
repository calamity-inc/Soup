#include "audSound.hpp"

#include <cfloat>

#include "aud_common.hpp"

namespace soup
{
	bool audSound::hasFinished() noexcept
	{
		return false;
	}

	bool audSoundSimple::hasFinished() noexcept
	{
		return t >= getDurationSeconds();
	}

	double audSoundSimple::getAmplitude()
	{
		t += AUD_TIME_STEP;
		return getAmplitude(t);
	}

	double audSoundSimple::getDurationSeconds() noexcept
	{
		return DBL_MAX;
	}
}
