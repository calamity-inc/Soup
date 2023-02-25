#pragma once

#include <string>
#include <vector>

namespace soup
{
	struct Compiler
	{
		std::string prog;
		std::string prog_ar;
		std::string lang; // defaults to "c++20" or "c++17" depending on platform
		bool rtti = false;
		std::vector<std::string> extra_args{};
		std::vector<std::string> extra_linker_args{};

		Compiler();

		[[nodiscard]] std::vector<std::string> getArgs() const;
		void addLinkerArgs(std::vector<std::string>& args) const;

		// Intermediate objects (.o)
		std::string makeObject(const std::string& in, const std::string& out) const;

		// Executables (.exe)
		[[nodiscard]] static const char* getExecutableExtension() noexcept; // ".exe" or ""
		std::string makeExecutable(const std::string& in, const std::string& out) const;
		std::string makeExecutable(const std::vector<std::string>& objects, const std::string& out) const;

		// Static libraries
		[[nodiscard]] static const char* getStaticLibraryExtension() noexcept; // ".lib" or ".a"
		std::string makeStaticLibrary(const std::vector<std::string>& objects, const std::string& out) const;

		// Dynamic / shared libraries
		[[nodiscard]] static const char* getDynamicLibraryExtension() noexcept; // ".dll" or ".so"
		std::string makeDynamicLibrary(const std::string& in, const std::string& out) const;
		std::string makeDynamicLibrary(const std::vector<std::string>& objects, const std::string& out) const;
	};
}
