#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include <atomic>

#include "aud_common.hpp"
#include "audDevice.hpp"
#include "Thread.hpp"

#if SOUP_WINDOWS
#include <Windows.h>
#include <mmeapi.h>
#else
#include <alsa/asoundlib.h> // sudo apt install libasound2-dev
#pragma comment(lib, "asound")
#endif

namespace soup
{
	class audPlayback
	{
	private:
		std::atomic_int free_blocks = 0;
		int current_block = 0;
		void* const heap;
		double time = 0.0;

	public:
		audDevice dev;
		int channels;
		audFillBlock src = &fillBlockSilenceSrc;
		void* user_data;

		Thread thrd;
#if SOUP_WINDOWS
		HWAVEOUT hWaveOut = INVALID_HANDLE_VALUE;
#else
		snd_pcm_t* hwDevice = nullptr;
#endif

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
