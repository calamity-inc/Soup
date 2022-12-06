#include "audPlayback.hpp"

#if SOUP_WINDOWS

#include <chrono>
#include <thread>

namespace soup
{
	// Kudos to https://github.com/OneLoneCoder/synth and https://gist.github.com/seungin/4779216eada24a5077ca1c5e857239ce

	using sample_t = int16_t;

	constexpr int SAMPLE_RATE = 44100;
	constexpr int CHANNELS = 1;
	constexpr int NUM_BLOCKS = 8;
	constexpr int BLOCK_SAMPLES = 512;

	constexpr int BLOCK_BUFFER_BYTES = (BLOCK_SAMPLES * sizeof(sample_t));
	constexpr int HEAP_SIZE = (NUM_BLOCKS * (sizeof(WAVEHDR) + BLOCK_BUFFER_BYTES));
	constexpr double TIME_STEP = (1.0 / (double)SAMPLE_RATE);
	constexpr sample_t MAX_SAMPLE = ((1ull << ((sizeof(sample_t) * 8) - 1)) - 1);

	audPlayback::audPlayback(const audDevice& dev, audGetAmplitude src, void* user_data)
		: heap(malloc(HEAP_SIZE)),
		dev(dev), src(src), user_data(user_data),
		thrd(&threadFuncStatic, this)
	{
		ZeroMemory(heap, HEAP_SIZE);

		for (auto i = 0; i != NUM_BLOCKS; ++i)
		{
			auto hdr = heapGetHeader(i);
			hdr->dwBufferLength = BLOCK_BUFFER_BYTES;
			hdr->lpData = reinterpret_cast<LPSTR>(heapGetBuffer(i));
		}

		WAVEFORMATEX wfx;
		wfx.wFormatTag = WAVE_FORMAT_PCM;
		wfx.nSamplesPerSec = SAMPLE_RATE;
		wfx.wBitsPerSample = sizeof(sample_t) * 8;
		wfx.nChannels = CHANNELS;
		wfx.nBlockAlign = (wfx.wBitsPerSample * CHANNELS) >> 3;
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
		wfx.cbSize = 0;

		waveOutOpen(&hWaveOut, dev.i, &wfx, reinterpret_cast<DWORD_PTR>(&waveCallbackStatic), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION);
	}

	audPlayback::~audPlayback()
	{
		free(heap);
	}

	void audPlayback::awaitCompletion() noexcept
	{
		thrd.awaitCompletion();
	}

	void audPlayback::stop() noexcept
	{
		thrd.stop();
	}

	WAVEHDR* audPlayback::heapGetHeader(int i) const noexcept
	{
		return reinterpret_cast<WAVEHDR*>(reinterpret_cast<uintptr_t>(heap) + (i * sizeof(WAVEHDR)));
	}

	void* audPlayback::heapGetBuffer(int i) const noexcept
	{
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(heap) + (NUM_BLOCKS * sizeof(WAVEHDR)) + (i * BLOCK_BUFFER_BYTES));
	}

	void audPlayback::threadFuncStatic(Capture&& cap)
	{
		cap.get<audPlayback*>()->threadFunc();
	}

	void audPlayback::threadFunc()
	{
		while (true)
		{
			while (free_blocks == 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			auto hdr = heapGetHeader(current_block);
			auto buf = heapGetBuffer(current_block);

			if (hdr->dwFlags & WHDR_PREPARED)
			{
				waveOutUnprepareHeader(hWaveOut, hdr, sizeof(WAVEHDR));
			}

			if (on_begin_writing_block)
			{
				on_begin_writing_block(*this);
			}

			for (int i = 0; i != BLOCK_SAMPLES; ++i)
			{
				reinterpret_cast<sample_t*>(buf)[i] = static_cast<sample_t>(src(*this) * MAX_SAMPLE);
				time += TIME_STEP;
			}

			waveOutPrepareHeader(hWaveOut, hdr, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, hdr, sizeof(WAVEHDR));

			--free_blocks;
			current_block += 1;
			current_block %= NUM_BLOCKS;
		}
	}

	__stdcall void audPlayback::waveCallbackStatic(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
	{
		reinterpret_cast<audPlayback*>(dwInstance)->waveCallback(uMsg);
	}

	void audPlayback::waveCallback(UINT msg)
	{
		if (msg == WOM_OPEN)
		{
			free_blocks = NUM_BLOCKS;
		}
		else if (msg == WOM_DONE)
		{
			++free_blocks;
		}
	}
}

#endif
