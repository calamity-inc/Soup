#pragma once

#include <string>
#include <vector>

namespace soup
{
	struct Compiler
	{
		std::string lang; // defaults to "c++20" or "c++17" depending on platform
		bool rtti = false;
		std::vector<std::string> extra_args{};
		std::vector<std::string> extra_linker_args{};

		Compiler();

		[[nodiscard]] std::vector<std::string> getArgs() const;
		[[nodiscard]] std::vector<std::string> getLinkerArgs() const;

		// Intermediate objects (.o)
		std::string makeObject(const std::string& in, const std::string& out) const;

		// Executables (.exe)
		[[nodiscard]] static const char* getExecutableExtension() noexcept; // ".exe" or ""
		std::string makeExecutable(const std::string& in, const std::string& out) const;
		std::string makeExecutable(const std::vector<std::string>& objects, const std::string& out) const;

		// Static libraries
		[[nodiscard]] static const char* getStaticLibraryExtension() noexcept; // ".lib" or ".a"
		std::string makeStaticLibrary(const std::vector<std::string>& objects, const std::string& out) const;

		// Shared / dynamic link libraries
		[[nodiscard]] static const char* getSharedLibraryExtension() noexcept; // ".dll" or ".so"
		std::string makeSharedLibrary(const std::string& in, const std::string& out) const;
	};
}
