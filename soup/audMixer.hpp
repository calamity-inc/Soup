#pragma once

#include "base.hpp"
#if SOUP_WINDOWS
#include "fwd.hpp"

#include <mutex>
#include <vector>

#include "SharedPtr.hpp"

namespace soup
{
	class audMixer
	{
	public:
		std::mutex mtx{};
		std::vector<SharedPtr<audSound>> playing_sounds{};
		bool stop_playback_when_done = false;
		bool kill_pb_on_next_block = false;

		void setOutput(audPlayback& pb);

		void playSound(SharedPtr<audSound> sound);

	protected:
		[[nodiscard]] double getAmplitude(audPlayback& pb) noexcept;
	};
}
#endif
