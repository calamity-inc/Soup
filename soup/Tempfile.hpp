#pragma once

#include "FileRaii.hpp"

NAMESPACE_SOUP
{
	struct Tempfile : public FileRaii
	{
		explicit Tempfile(const std::string& ext);
	};
}
