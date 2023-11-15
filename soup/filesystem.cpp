#include "filesystem.hpp"

#include "base.hpp"

namespace soup
{
	bool filesystem::exists_case_sensitive(const std::filesystem::path& p)
	{
		return std::filesystem::exists(p)
#if SOUP_WINDOWS
			&& p.filename() == std::filesystem::canonical(p).filename()
#endif
			;
	}
}
