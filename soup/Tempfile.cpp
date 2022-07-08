#include "Tempfile.hpp"

#include "os.hpp"

namespace soup
{
	Tempfile::Tempfile(const std::string& ext)
		: FileRaii(os::tempfile(ext))
	{
	}
}
