#include "Tempfile.hpp"

#include "filesystem.hpp"

NAMESPACE_SOUP
{
	Tempfile::Tempfile(const std::string& ext)
		: FileRaii(filesystem::tempfile(ext))
	{
	}
}
