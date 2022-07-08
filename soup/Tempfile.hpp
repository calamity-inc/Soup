#pragma once

#include "FileRaii.hpp"

namespace soup
{
	struct Tempfile : public FileRaii
	{
		explicit Tempfile(const std::string& ext);
	};
}
