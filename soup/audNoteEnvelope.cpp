#include "audNoteEnvelope.hpp"

NAMESPACE_SOUP
{
	void audNoteEnvelope::prepare()
	{
		t = 0.0;
		time_released = 0.0;
		released = false;
	}

	bool audNoteEnvelope::hasFinished() noexcept
	{
		if (time_released != 0.0)
		{
			const double time_since_release = (t - time_released);
			return time_since_release > release_time;
		}
		return false;
	}

	double audNoteEnvelope::getAmplitude()
	{
		t += AUD_TIME_STEP;

		double amplitude = sin(angvel * t);

		if (t <= attack_time) // Attack
		{
			amplitude *= (t / attack_time);
		}
		else if (t <= attack_time + decay_time) // Decay
		{
			amplitude *= (1.0 - (sustain_amplitude * ((t - attack_time) / decay_time)));
		}
		else if (time_released == 0.0) // Sustain
		{
			amplitude *= sustain_amplitude;
			if (released)
			{
				time_released = t;
			}
		}
		else
		{
			const double time_since_release = (t - time_released);
			amplitude *= (sustain_amplitude * (1.0 - (time_since_release / release_time)));
		}

		// Reduce volume to allow multiple audNoteEnvelope to play at once without undesirable sounds
		amplitude *= 0.2;

		return amplitude;
	}
}
