#pragma once

#include "base.hpp"
#if !SOUP_WASM || SOUP_EMSCRIPTEN
#include "fwd.hpp"

#include <cstdint>
#include <string>

NAMESPACE_SOUP
{
	struct netAs
	{
		uint32_t number;
		const char* handle;
		const char* name;

		// Does the AS belong to a hosting provider? Can be used to tell if this is a VPN.
		[[nodiscard]] bool isHosting(const netIntel& intel) const SOUP_EXCAL;
	};
}

#endif
