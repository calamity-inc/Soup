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
		// Some audSound descendents like audWav don't respect `t`.
		for (const auto& ps : playing_sounds)
		{
			if (ps.get() == sound.get())
			{
				SOUP_THROW(Exception("Sound is already playing"));
			}
		}

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
		return a;
	}
}

#endif
