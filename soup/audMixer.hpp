#pragma once

#include "base.hpp"
#if SOUP_WINDOWS
#include "fwd.hpp"

#include <vector>

namespace soup
{
	class audMixer
	{
	public:
		struct PlayingSound
		{
			audSound* sound;
			bool loop;
			double start = 0.0;
		};

		std::vector<PlayingSound> playing_sounds{};
		bool stop_playback_when_no_sounds_are_playing = false;
		bool kill_pb_on_next_block = false;

		void setOutput(audPlayback& pb);

		void playSound(audSound* sound, bool loop = false);

	protected:
		[[nodiscard]] double getAmplitude(audPlayback& pb) noexcept;
	};
}
#endif
