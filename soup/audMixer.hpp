#pragma once

#include "base.hpp"
#if (SOUP_WINDOWS && !SOUP_CROSS_COMPILE) || SOUP_LINUX
#include "fwd.hpp"

#include <vector>

#include "audPlayback.hpp"
#include "Mutex.hpp"
#include "SharedPtr.hpp"
#include "WeakRef.hpp"

NAMESPACE_SOUP
{
	class audMixer
	{
	protected:
		WeakRef<audPlayback> attached_to_pb{};
	public:
		Mutex mtx{};
		std::vector<SharedPtr<audSound>> playing_sounds{};
		bool stop_playback_when_done = false;

		~audMixer();

		void setOutput(audPlayback& pb);

		void playSound(SharedPtr<audSound> sound);

	protected:
		[[nodiscard]] double getAmplitude(audPlayback& pb) noexcept;
	};
}

#endif
