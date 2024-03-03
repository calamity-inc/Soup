#include "Tempfile.hpp"

#include "filesystem.hpp"

namespace soup
{
	Tempfile::Tempfile(const std::string& ext)
		: FileRaii(filesystem::tempfile(ext))
	{
	}
}
