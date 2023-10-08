#pragma once

#include "base.hpp"
#if SOUP_WINDOWS
#include "fwd.hpp"

#include <vector>

#include "SharedPtr.hpp"

namespace soup
{
	class audMixer
	{
	public:
		struct PlayingSound
		{
			SharedPtr<audSound> sound;
			bool loop;
			double start = 0.0;
		};

		std::vector<PlayingSound> playing_sounds{};
		bool stop_playback_when_done = false;
		bool kill_pb_on_next_block = false;

		void setOutput(audPlayback& pb);

		void playSound(SharedPtr<audSound> sound, bool loop = false);

	protected:
		[[nodiscard]] double getAmplitude(audPlayback& pb) noexcept;
	};
}
#endif
