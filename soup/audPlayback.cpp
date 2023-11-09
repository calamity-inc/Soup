#include "audPlayback.hpp"
#if !SOUP_WASM

#include <atomic>
#include <chrono>
#include <thread>

#if SOUP_LINUX
#include <alsa/asoundlib.h> // sudo apt install libasound2-dev
#pragma comment(lib, "asound")
#endif

namespace soup
{
	// Kudos to:
	// - https://github.com/OneLoneCoder/synth
	// - https://gist.github.com/seungin/4779216eada24a5077ca1c5e857239ce
	// - https://github.com/Barracuda72/synth/blob/master/LinuxSoundSynthesizer/NoiseMaker.h

	constexpr int NUM_BLOCKS = 8;

#if SOUP_WINDOWS
	constexpr int HEAP_SIZE = (NUM_BLOCKS * (sizeof(WAVEHDR) + AUD_BLOCK_BYTES));
#else
	constexpr int HEAP_SIZE = (NUM_BLOCKS * AUD_BLOCK_BYTES);
#endif

	audPlayback::audPlayback()
		: heap(malloc(HEAP_SIZE)), thrd(&threadFuncStatic, this)
	{
		memset(heap, 0, HEAP_SIZE);

#if SOUP_WINDOWS
		for (auto i = 0; i != NUM_BLOCKS; ++i)
		{
			auto hdr = heapGetHeader(i);
			hdr->dwBufferLength = AUD_BLOCK_BYTES;
			hdr->lpData = reinterpret_cast<LPSTR>(heapGetBuffer(i));
		}
#endif
	}

	audPlayback::~audPlayback()
	{
		thrd.stop();
#if SOUP_WINDOWS
		if (hWaveOut != INVALID_HANDLE_VALUE)
		{
			waveOutReset(hWaveOut);
			waveOutClose(hWaveOut);
		}
#else
		if (hwDevice != nullptr)
		{
			snd_pcm_drop(hwDevice);
			snd_pcm_close(hwDevice);
		}
#endif
		free(heap);
	}

#if SOUP_WINDOWS
	static void CALLBACK waveCallbackStatic(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
	{
		reinterpret_cast<audPlayback*>(dwInstance)->handleMessage(uMsg);
	}
#else
	static void waveCallbackStatic(snd_async_handler_t* ahandler)
	{
		reinterpret_cast<audPlayback*>(snd_async_handler_get_callback_private(ahandler))->maintainPcm();
	}
#endif

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
#if SOUP_WINDOWS
		SOUP_ASSERT(hWaveOut == INVALID_HANDLE_VALUE);
#else
		SOUP_ASSERT(hwDevice == nullptr);
#endif

		this->dev = dev;
		this->channels = channels;
		this->src = src;
		this->user_data = user_data;

#if SOUP_WINDOWS
		WAVEFORMATEX wfx;
		wfx.wFormatTag = WAVE_FORMAT_PCM;
		wfx.nSamplesPerSec = AUD_SAMPLE_RATE;
		wfx.wBitsPerSample = sizeof(audSample) * 8;
		wfx.nChannels = channels;
		wfx.nBlockAlign = sizeof(audSample) * channels;
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
		wfx.cbSize = 0;
		waveOutOpen(&hWaveOut, dev.i, &wfx, reinterpret_cast<DWORD_PTR>(&waveCallbackStatic), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION);
#else
		// BUG: snd_pcm_hw_params_free is not called in exception case
		snd_pcm_hw_params_t* hw_params;
		snd_pcm_open(&hwDevice, "default", SND_PCM_STREAM_PLAYBACK, 0);
		snd_pcm_hw_params_malloc(&hw_params);
		snd_pcm_hw_params_any(hwDevice, hw_params);
		SOUP_ASSERT(snd_pcm_hw_params_set_access(hwDevice, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED) >= 0);
		snd_async_handler_t* handler;
		SOUP_ASSERT(snd_async_add_pcm_handler(&handler, hwDevice, waveCallbackStatic, this) >= 0, "ALSA init failed. You need to be root. Don't ask me why, because I don't know either.");
		snd_pcm_hw_params_set_format(hwDevice, hw_params, SND_PCM_FORMAT_S16_LE); static_assert(std::is_same_v<int16_t, audSample>);
		unsigned int sample_rate = AUD_SAMPLE_RATE;
		snd_pcm_hw_params_set_rate_near(hwDevice, hw_params, &sample_rate, 0);
		SOUP_ASSERT(sample_rate == AUD_SAMPLE_RATE);
		snd_pcm_hw_params_set_channels(hwDevice, hw_params, channels);
		snd_pcm_hw_params(hwDevice, hw_params);
		snd_pcm_hw_params_free(hw_params);
		SOUP_ASSERT(snd_pcm_prepare(hwDevice) >= 0);
		free_blocks = NUM_BLOCKS;
#endif
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

#if SOUP_WINDOWS
	void audPlayback::handleMessage(UINT msg)
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
#else
	void audPlayback::maintainPcm()
	{
		if (snd_pcm_state(hwDevice) == SND_PCM_STATE_XRUN)
        {
        	SOUP_ASSERT(snd_pcm_prepare(hwDevice) >= 0);
        }

		++free_blocks;
	}
#endif

#if SOUP_WINDOWS
	WAVEHDR* audPlayback::heapGetHeader(int i) const noexcept
	{
		return reinterpret_cast<WAVEHDR*>(reinterpret_cast<uintptr_t>(heap) + (i * sizeof(WAVEHDR)));
	}
#endif

	void* audPlayback::heapGetBuffer(int i) const noexcept
	{
#if SOUP_WINDOWS
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(heap) + (NUM_BLOCKS * sizeof(WAVEHDR)) + (i * AUD_BLOCK_BYTES));
#else
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(heap) + (i * AUD_BLOCK_BYTES));
#endif
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

#if SOUP_WINDOWS
			auto hdr = heapGetHeader(current_block);
			if (hdr->dwFlags & WHDR_PREPARED)
			{
				waveOutUnprepareHeader(hWaveOut, hdr, sizeof(WAVEHDR));
			}
#endif

			auto buf = heapGetBuffer(current_block);
			src(*this, reinterpret_cast<audSample*>(buf));

#if SOUP_WINDOWS
			waveOutPrepareHeader(hWaveOut, hdr, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, hdr, sizeof(WAVEHDR));
#else
			snd_pcm_writei(hwDevice, buf, AUD_BLOCK_SAMPLES);
#endif

			--free_blocks;
			current_block += 1;
			current_block %= NUM_BLOCKS;
		}
	}
}

#endif
