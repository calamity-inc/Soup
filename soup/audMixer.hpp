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
	public:
		struct Input
		{
			SharedPtr<audSound> sound;
			double amplitude;
		};

	protected:
		WeakRef<audPlayback> attached_to_pb{};
	public:
		Mutex mtx{};
		std::vector<Input> playing_sounds{};
		bool stop_playback_when_done = false;

		~audMixer();

		void setOutput(audPlayback& pb);

		void playSound(SharedPtr<audSound> sound, double amplitude = 1.0);

	protected:
		[[nodiscard]] double getAmplitude(audPlayback& pb) noexcept;
	};
}

#endif
