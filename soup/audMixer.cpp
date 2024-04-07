#include "audMixer.hpp"
#if SOUP_WINDOWS || SOUP_LINUX

#include <algorithm>

#include "audPlayback.hpp"
#include "audSound.hpp"

NAMESPACE_SOUP
{
	void audMixer::setOutput(audPlayback& pb)
	{
		pb.src = [](audPlayback& pb, audSample* block)
		{
			std::lock_guard lock(reinterpret_cast<audMixer*>(pb.user_data)->mtx);
			if (reinterpret_cast<audMixer*>(pb.user_data)->stop_playback_when_done
				&& reinterpret_cast<audMixer*>(pb.user_data)->playing_sounds.empty()
				)
			{
				pb.fillBlockSilence(block);
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
		std::lock_guard lock(mtx);

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
		// mtx should be locked

		double a = 0.0;
		for (auto i = playing_sounds.begin(); i != playing_sounds.end(); )
		{
			SOUP_IF_UNLIKELY ((*i)->hasFinished())
			{
				i = playing_sounds.erase(i);
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
