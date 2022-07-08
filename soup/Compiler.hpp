#pragma once

#include <string>

namespace soup
{
	struct Compiler
	{
		// Executables (.exe)
		static std::string compileExecutable(const std::string& in, const std::string& out);

		// Shared / dynamic link libraries (.dll, .so)
		[[nodiscard]] static const char* getSharedLibraryExtension() noexcept;
		static std::string compileSharedLibrary(const std::string& in, const std::string& out);
	};
}
