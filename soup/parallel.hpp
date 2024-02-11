#pragma once

#include "base.hpp"
#if !SOUP_WASM

#include "Capture.hpp"

namespace soup
{
	struct parallel
	{
		static void iterateRange(unsigned int size, void(*callback)(unsigned int, const Capture&), const Capture& cap = {});
	};
}

#endif
