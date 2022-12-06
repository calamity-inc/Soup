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

		void setOutput(audPlayback& pb);

		void playSound(audSound* sound, bool loop = false);

	protected:
		[[nodiscard]] double getAmplitude(double t) noexcept;
	};
}
#endif
