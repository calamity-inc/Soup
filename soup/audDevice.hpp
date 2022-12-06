#pragma once

#include "base.hpp"

#if SOUP_WINDOWS
#pragma comment(lib, "winmm.lib")

#include "fwd.hpp"

#include "math.hpp"
#define HZ_TO_ANGVEL(x) ((x) * 2 * M_PI)

#include <string>
#include <vector>

namespace soup
{
	// Return the amplitude (-1.0 to +1.0) at a given point in time (audPlayback::getTime)
	using audGetAmplitude = double(*)(audPlayback&);

	class audDevice
	{
	public:
		int i;
	private:
		std::wstring name;

	public:
		audDevice(int i, std::wstring&& name)
			: i(i), name(std::move(name))
		{
		}

		[[nodiscard]] static audDevice get(int i);
		[[nodiscard]] static std::vector<audDevice> getAll();

		[[nodiscard]] std::string getName() const;
		UniquePtr<audPlayback> open(audGetAmplitude src) const;
	};
}

#endif
