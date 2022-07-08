#pragma once

#include <filesystem>
#include <string>

#include "Tempfile.hpp"

namespace soup
{
	struct CompiledExecutable
	{
		Tempfile exe_file;
		std::string compiler_output;

		[[nodiscard]] static CompiledExecutable fromCpp(const FileRaii& file);
		[[nodiscard]] static CompiledExecutable fromCpp(const std::filesystem::path& path);
		[[nodiscard]] static CompiledExecutable fromCpp(const char* path);
		[[nodiscard]] static CompiledExecutable fromCpp(const std::string& path);
	};
}
