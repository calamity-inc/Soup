#include "audMixer.hpp"
#if SOUP_WINDOWS

#include <algorithm>

#include "audPlayback.hpp"
#include "audSound.hpp"

namespace soup
{
	void audMixer::setOutput(audPlayback& pb)
	{
		pb.src = [](audPlayback& pb, audSample* block)
		{
			if (reinterpret_cast<audMixer*>(pb.user_data)->kill_pb_on_next_block)
			{
				pb.stop();
				return;
			}
			pb.fillBlockImpl(block, [](audPlayback& pb)
			{
				return reinterpret_cast<audMixer*>(pb.user_data)->getAmplitude(pb);
			});
		};
		pb.user_data = this;
	}

	void audMixer::playSound(SharedPtr<audSound> sound)
	{
		for (const auto& ps : playing_sounds)
		{
			if (ps.get() == sound.get())
			{
				SOUP_THROW(Exception("Sound is already playing"));
			}
		}

		sound->prepare();

		playing_sounds.emplace_back(std::move(sound));
	}

	double audMixer::getAmplitude(audPlayback& pb) noexcept
	{
		double a = 0.0;
		for (auto i = playing_sounds.begin(); i != playing_sounds.end(); )
		{
			SOUP_IF_UNLIKELY ((*i)->hasFinished())
			{
				i = playing_sounds.erase(i);
				if (stop_playback_when_done
					&& playing_sounds.empty()
					)
				{
					kill_pb_on_next_block = true;
				}
				continue;
			}
			a += (*i)->getAmplitude();
			++i;
		}

		// Prevent clipping if too many sounds are playing (at full volume)
		a = std::clamp(a, -1.0, 1.0);

		return a;
	}
}

#endif
