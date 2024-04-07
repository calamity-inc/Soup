#pragma once

#include "base.hpp"
#if SOUP_WINDOWS || SOUP_LINUX
#include "fwd.hpp"

#include <cstdint>
#include <string>
#include <vector>

#if SOUP_WINDOWS
#pragma comment(lib, "winmm.lib")
#endif

NAMESPACE_SOUP
{
	using audSample = int16_t;

	constexpr int AUD_BLOCK_SAMPLES = 1024;
	constexpr int AUD_BLOCK_BYTES = (AUD_BLOCK_SAMPLES * sizeof(audSample));

	constexpr audSample AUD_MAX_SAMPLE = ((1ull << ((sizeof(audSample) * 8) - 1)) - 1);

	using audFillBlock = void(*)(audPlayback&, audSample* block);

	// Return the amplitude (-1.0 to +1.0) at a given point in time (audPlayback::getTime)
	using audGetAmplitude = double(*)(audPlayback&);

	class audDevice
	{
	public:
		unsigned int i;
	private:
#if SOUP_WINDOWS
		std::wstring name;
#endif
	public:
		uint16_t max_channels;

#if SOUP_WINDOWS
		audDevice() = default;

		audDevice(unsigned int i, std::wstring&& name, uint16_t max_channels)
			: i(i), name(std::move(name)), max_channels(max_channels)
		{
		}
#else
		audDevice()
			: i(0), max_channels(2)
		{
		}
#endif

		[[nodiscard]] static audDevice get(unsigned int i);
		[[nodiscard]] static audDevice getDefault();
		[[nodiscard]] static std::vector<audDevice> getAll();

		[[nodiscard]] std::string getName() const;
		[[nodiscard]] UniquePtr<audPlayback> open(int channels = 1) const;
		UniquePtr<audPlayback> open(audFillBlock src, void* user_data = nullptr) const;
		UniquePtr<audPlayback> open(int channels, audFillBlock src, void* user_data = nullptr) const;
	};
}

#endif
