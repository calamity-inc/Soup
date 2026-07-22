#pragma once

#include "base.hpp"

NAMESPACE_SOUP
{
	// Generic access to any provider of content-addressed data
	struct cadInterface
	{
		// Empty string if no root object.
		[[nodiscard]] virtual std::string getRootHash() = 0;

		[[nodiscard]] virtual std::string getContent(const std::string& hash) = 0;

		//[[nodiscard]] virtual void getContentAsync(const std::string& hash, void callback(const std::string& hash, const std::string& content, Capture&&), Capture&& cap = {}) = 0;
	};
}
