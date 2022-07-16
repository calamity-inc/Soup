#pragma once

#include <string>
#include <vector>

namespace soup
{
	struct Compiler
	{
		// Intermediate objects (.o)
		static std::string makeObject(const std::string& in, const std::string& out);

		// Executables (.exe)
		[[nodiscard]] static const char* getExecutableExtension() noexcept; // ".exe" or ""
		static std::string makeExecutable(const std::string& in, const std::string& out);
		static std::string makeExecutable(const std::vector<std::string>& objects, const std::string& out);

		// Static libraries
		[[nodiscard]] static const char* getStaticLibraryExtension() noexcept; // ".lib" or ".a"
		static std::string makeStaticLibrary(const std::vector<std::string>& objects, const std::string& out);

		// Shared / dynamic link libraries
		[[nodiscard]] static const char* getSharedLibraryExtension() noexcept; // ".dll" or ".so"
		static std::string makeSharedLibrary(const std::string& in, const std::string& out);
	};
}
