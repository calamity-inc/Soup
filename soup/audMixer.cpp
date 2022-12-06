#include "audMixer.hpp"

#if SOUP_WINDOWS

#include "audPlayback.hpp"
#include "audSound.hpp"

namespace soup
{
	void audMixer::setOutput(audPlayback& pb)
	{
		pb.src = [](audPlayback& pb, audSample* block)
		{
			pb.fillBlockImpl(block, [](audPlayback& pb)
			{
				return reinterpret_cast<audMixer*>(pb.user_data)->getAmplitude(pb);
			});
		};
		pb.user_data = this;
	}

	void audMixer::playSound(audSound* sound, bool loop)
	{
		playing_sounds.emplace_back(PlayingSound{ sound, loop });
	}

	double audMixer::getAmplitude(audPlayback& pb) noexcept
	{
		const double t = pb.getTime();

		double a = 0.0;
		for (auto i = playing_sounds.begin(); i != playing_sounds.end(); )
		{
			if (i->start == 0.0)
			{
				i->start = t;
			}
			double t_for_sound = (t - i->start);
			if (t_for_sound >= i->sound->getDurationSeconds())
			{
				if (!i->loop)
				{
					i = playing_sounds.erase(i);
					if (stop_playback_when_no_sounds_are_playing
						&& playing_sounds.empty()
						)
					{
						pb.stop();
					}
					continue;
				}
				i->start = t;
				t_for_sound = 0.0;
			}
			a += i->sound->getAmplitude(t_for_sound);
			++i;
		}
		return a;
	}
}

#endif
