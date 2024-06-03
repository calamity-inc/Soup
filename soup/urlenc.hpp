#pragma once

#include <string>

#include "base.hpp" // SOUP_EXCAL

NAMESPACE_SOUP
{
	struct urlenc
	{
		[[nodiscard]] static std::string encode(const std::string& data) SOUP_EXCAL;
		[[nodiscard]] static std::string decode(const std::string& data) SOUP_EXCAL;
	};
}
