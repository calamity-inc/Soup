#pragma once

#include "audDevice.hpp"

#if SOUP_WINDOWS

#include <Windows.h>
#include <mmeapi.h>

#include <atomic>

#include "Notifyable.hpp"
#include "Thread.hpp"

namespace soup
{
	constexpr int AUD_SAMPLE_RATE = 44100;

	constexpr double AUD_TIME_STEP = (1.0 / (double)AUD_SAMPLE_RATE);

	class audPlayback
	{
	private:
		std::atomic_int free_blocks = 0;
		Notifyable block_available;
		int current_block = 0;
		void* const heap;
		double time = 0.0;

	public:
		audDevice dev;
		int channels;
		audFillBlock src = &fillBlockSilenceSrc;
		void* user_data;

		Thread thrd;
		HWAVEOUT hWaveOut;

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

		[[nodiscard]] bool isRunning() const noexcept;
		void awaitCompletion() noexcept;
		void stop() noexcept;
		
		static void fillBlockSilence(audSample* block);
		static void fillBlockSilenceSrc(audPlayback&, audSample* block);
		void fillBlockImpl(audSample* block, audGetAmplitude src);

	private:
		[[nodiscard]] WAVEHDR* heapGetHeader(int i) const noexcept;
		[[nodiscard]] void* heapGetBuffer(int i) const noexcept;

		static void threadFuncStatic(Capture&& cap);
		void threadFunc();

		static __stdcall void waveCallbackStatic(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
		void waveCallback(UINT msg);
	};
}

#endif
