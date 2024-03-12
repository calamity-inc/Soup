#pragma once

#include "base.hpp"
#include "fwd.hpp"

#include <cstdint>
#include <string>

namespace soup
{
	class netAs
	{
	public:
		uint32_t number;
		const char* handle;
		const char* name;

		netAs() = default;

		netAs(uint32_t number, const char* name)
			: number(number), handle(name), name(name)
		{
		}

		// Does the AS belong to a hosting provider? Can be used to tell if this is a VPN.
		[[nodiscard]] bool isHosting(const netIntel& intel) const SOUP_EXCAL;
		[[deprecated("Provide your netIntel instance as an argument for more up-to-date results.")]] bool isHosting() const SOUP_EXCAL;
	private:
		[[nodiscard]] bool isHosting(const std::string& extra_wasm) const SOUP_EXCAL;
	};
}
