#pragma once

#include "base.hpp"

#if SOUP_WINDOWS
#pragma comment(lib, "winmm.lib")

#include "fwd.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace soup
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
		int i;
	private:
		std::wstring name;
	public:
		uint16_t max_channels;

		audDevice(int i, std::wstring&& name, uint16_t max_channels)
			: i(i), name(std::move(name)), max_channels(max_channels)
		{
		}

		[[nodiscard]] static audDevice get(int i);
		[[nodiscard]] static audDevice getDefault();
		[[nodiscard]] static std::vector<audDevice> getAll();

		[[nodiscard]] std::string getName() const;
		[[nodiscard]] UniquePtr<audPlayback> open(int channels = 1) const;
		UniquePtr<audPlayback> open(audFillBlock src, void* user_data = nullptr) const;
		UniquePtr<audPlayback> open(int channels, audFillBlock src, void* user_data = nullptr) const;
	};
}

#endif
