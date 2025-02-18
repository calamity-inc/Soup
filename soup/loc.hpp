#pragma once

#include "base.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

NAMESPACE_SOUP
{
	struct loc
	{
		[[nodiscard]] static std::unordered_map<uint32_t, std::string> getDict(std::string lang_code);
		[[nodiscard]] static std::unordered_map<uint32_t, std::string> loadDict(const char* data, size_t size);
	};
}
