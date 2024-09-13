#pragma once

#include "base.hpp"
#if (SOUP_WINDOWS && !SOUP_CROSS_COMPILE) || SOUP_LINUX

#include <atomic>

#include "aud_common.hpp"
#include "audDevice.hpp"
#include "Thread.hpp"
#include "TransientToken.hpp"

#if SOUP_WINDOWS
#include <windows.h>
#include <mmeapi.h>
#else
#include "SharedLibrary.hpp"
#endif

NAMESPACE_SOUP
{
#if SOUP_LINUX
	using snd_pcm_t = void;
	using snd_pcm_sframes_t = long;
	using snd_pcm_uframes_t = unsigned long;
	using snd_pcm_state_t = int;

	using snd_pcm_state_func_t = snd_pcm_state_t(*)(snd_pcm_t*);
	using snd_pcm_prepare_t = int(*)(snd_pcm_t*);
	using snd_pcm_writei_t = snd_pcm_sframes_t(*)(snd_pcm_t*, const void*, snd_pcm_uframes_t);
#endif

	class audPlayback
	{
	private:
		std::atomic_int free_blocks = 0;
		int current_block = 0;
		bool stopping = false;
		void* const heap;
		double time = 0.0;

	public:
		audDevice dev;
		int channels;
		audFillBlock src = &fillBlockSilenceSrc;
		void* user_data;

		Thread thrd;
#if SOUP_WINDOWS
		HWAVEOUT hWaveOut = nullptr;
#else
		SharedLibrary alsa;
		snd_pcm_state_func_t snd_pcm_state = nullptr;
		snd_pcm_prepare_t snd_pcm_prepare = nullptr;
		snd_pcm_writei_t snd_pcm_writei = nullptr;
		snd_pcm_t* hwDevice = nullptr;
#endif

		TransientToken transient_token;

		audPlayback();
		audPlayback(audPlayback&&) = delete; // `this` pointer must stay the same
		~audPlayback();

		void open(const audDevice& dev, int channels = 1);
		void open(const audDevice& dev, audFillBlock src, void* user_data = nullptr);
		void open(const audDevice& dev, int channels, audFillBlock src, void* user_data = nullptr);

		// Returns time in seconds since playback began.
		[[nodiscard]] constexpr double getTime() const noexcept
		{
			return time;
		}

		[[nodiscard]] bool isPlaying() const noexcept;
		void awaitCompletion() noexcept;
		void stop() noexcept;

		static void fillBlockSilence(audSample* block);
		static void fillBlockSilenceSrc(audPlayback&, audSample* block);
		void fillBlockImpl(audSample* block, audGetAmplitude src);

#if SOUP_WINDOWS
		void handleMessage(UINT msg);
#else
		void maintainPcm();
#endif

	private:
#if SOUP_WINDOWS
		[[nodiscard]] WAVEHDR* heapGetHeader(int i) const noexcept;
#endif
		[[nodiscard]] void* heapGetBuffer(int i) const noexcept;

		static void threadFuncStatic(Capture&& cap);
		void threadFunc();
	};
}

#endif
