#include "audPlayback.hpp"

#if SOUP_WINDOWS

#include <chrono>
#include <thread>

namespace soup
{
	// Kudos to https://github.com/OneLoneCoder/synth and https://gist.github.com/seungin/4779216eada24a5077ca1c5e857239ce

	constexpr int NUM_BLOCKS = 8;

	constexpr int HEAP_SIZE = (NUM_BLOCKS * (sizeof(WAVEHDR) + AUD_BLOCK_BYTES));

	audPlayback::audPlayback()
		: heap(malloc(HEAP_SIZE)), thrd(&threadFuncStatic, this)
	{
		ZeroMemory(heap, HEAP_SIZE);

		for (auto i = 0; i != NUM_BLOCKS; ++i)
		{
			auto hdr = heapGetHeader(i);
			hdr->dwBufferLength = AUD_BLOCK_BYTES;
			hdr->lpData = reinterpret_cast<LPSTR>(heapGetBuffer(i));
		}
	}

	audPlayback::~audPlayback()
	{
		free(heap);

		waveOutClose(hWaveOut);
	}

	static CALLBACK void waveCallbackStatic(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
	{
		reinterpret_cast<audPlayback*>(dwInstance)->waveCallback(uMsg);
	}

	void audPlayback::open(const audDevice& dev, int channels)
	{
		return open(dev, channels, &fillBlockSilenceSrc);
	}
	
	void audPlayback::open(const audDevice& dev, audFillBlock src, void* user_data)
	{
		return open(dev, 1, src, user_data);
	}

	void audPlayback::open(const audDevice& dev, int channels, audFillBlock src, void* user_data)
	{
		this->dev = dev;
		this->channels = channels;
		this->src = src;
		this->user_data = user_data;

		WAVEFORMATEX wfx;
		wfx.wFormatTag = WAVE_FORMAT_PCM;
		wfx.nSamplesPerSec = AUD_SAMPLE_RATE;
		wfx.wBitsPerSample = sizeof(audSample) * 8;
		wfx.nChannels = channels;
		wfx.nBlockAlign = sizeof(audSample) * channels;
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
		wfx.cbSize = 0;
		waveOutOpen(&hWaveOut, dev.i, &wfx, reinterpret_cast<DWORD_PTR>(&waveCallbackStatic), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION);
	}

	bool audPlayback::isPlaying() const noexcept
	{
		return thrd.isRunning()
			|| free_blocks != NUM_BLOCKS
			;
	}

	void audPlayback::awaitCompletion() noexcept
	{
		thrd.awaitCompletion();
		while (free_blocks != NUM_BLOCKS)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	void audPlayback::stop() noexcept
	{
		thrd.stop();
	}

	void audPlayback::fillBlockSilence(audSample* block)
	{
		memset(block, 0, AUD_BLOCK_BYTES);
	}

	void audPlayback::fillBlockSilenceSrc(audPlayback&, audSample* block)
	{
		fillBlockSilence(block);
	}

	void audPlayback::fillBlockImpl(audSample* block, audGetAmplitude src)
	{
		for (int i = 0; i != AUD_BLOCK_SAMPLES; i += channels)
		{
			for (int c = 0; c != channels; ++c)
			{
				block[i + c] = static_cast<audSample>(src(*this) * AUD_MAX_SAMPLE);
			}
			time += AUD_TIME_STEP;
		}
	}

	WAVEHDR* audPlayback::heapGetHeader(int i) const noexcept
	{
		return reinterpret_cast<WAVEHDR*>(reinterpret_cast<uintptr_t>(heap) + (i * sizeof(WAVEHDR)));
	}

	void* audPlayback::heapGetBuffer(int i) const noexcept
	{
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(heap) + (NUM_BLOCKS * sizeof(WAVEHDR)) + (i * AUD_BLOCK_BYTES));
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
				block_available.wait();
			}

			auto hdr = heapGetHeader(current_block);
			auto buf = heapGetBuffer(current_block);

			if (hdr->dwFlags & WHDR_PREPARED)
			{
				waveOutUnprepareHeader(hWaveOut, hdr, sizeof(WAVEHDR));
			}

			src(*this, reinterpret_cast<audSample*>(buf));

			waveOutPrepareHeader(hWaveOut, hdr, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, hdr, sizeof(WAVEHDR));

			--free_blocks;
			current_block += 1;
			current_block %= NUM_BLOCKS;
		}
	}

	void audPlayback::waveCallback(UINT msg)
	{
		if (msg == WOM_OPEN)
		{
			free_blocks = NUM_BLOCKS;
			block_available.notify_one();
		}
		else if (msg == WOM_DONE)
		{
			++free_blocks;
			block_available.notify_one();
		}
	}
}

#endif
