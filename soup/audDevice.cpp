#include "audDevice.hpp"

#if SOUP_WINDOWS

#include <Windows.h>
#include <mmeapi.h>

#include "audPlayback.hpp"
#include "unicode.hpp"

namespace soup
{
	audDevice audDevice::get(int i)
	{
		WAVEOUTCAPSW woc;
		SOUP_IF_UNLIKELY (waveOutGetDevCapsW(i, &woc, sizeof(WAVEOUTCAPSW)) != S_OK)
		{
			throw 0;
		}
		return audDevice{
			i,
			unicode::utf16_to_utf8<std::wstring>(woc.szPname)
		};
	}

	std::vector<audDevice> audDevice::getAll()
	{
		std::vector<audDevice> res;
		int num = waveOutGetNumDevs();
		for (int i = 0; i != num; ++i)
		{
			res.emplace_back(get(i));
		}
		return res;
	}

	UniquePtr<audPlayback> audDevice::open(audGetAmplitude src) const
	{
		return soup::make_unique<audPlayback>(*this, src);
	}
}

#endif
