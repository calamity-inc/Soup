#pragma once

#include <filesystem>

namespace soup
{
	struct filesystem
	{
		[[nodiscard]] static bool exists_case_sensitive(const std::filesystem::path& p);
	};
}
