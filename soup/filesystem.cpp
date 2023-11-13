#include "filesystem.hpp"

#include "base.hpp"

namespace soup
{
	bool filesystem::exists_case_sensitive(const std::filesystem::path& p)
	{
		return std::filesystem::exists(p)
#if SOUP_WINDOWS
			&& std::filesystem::absolute(p) == std::filesystem::canonical(p)
#endif
			;
	}
}
