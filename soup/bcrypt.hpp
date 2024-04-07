#pragma once

#include <string>

#include "base.hpp"

NAMESPACE_SOUP
{
	struct bcrypt
	{
		[[nodiscard]] static std::string hash(const std::string& password, unsigned rounds = 10);
		[[nodiscard]] static bool verify(const std::string& password, const std::string& hash);

		// bcrypt is limited to 71 bytes, so this function will modify input strings longer than 71 bytes such that:
		// - the first 39 bytes are identical
		// - bytes 39 and up are hashed, the result is that bytes 39 to 71 will contain their sha256 hash, which is 32 bytes.
		[[nodiscard]] static std::string preparePassword(const std::string& password);
	};
}
