#include "Compiler.hpp"

#include "base.hpp"

#include "os.hpp"

#if SOUP_WINDOWS
#define STD_ARG "-std=c++20"
#else
// Debian's at clang 11 right now, which does support C++20, but not enough to compile Soup without modifications.
#define STD_ARG "-std=c++17"
#endif

namespace soup
{
	std::string Compiler::compileExecutable(const std::string& in, const std::string& out)
	{
		return os::execute("clang", {
			STD_ARG,
#if !SOUP_WINDOWS
			"-lstdc++",
			"-lstdc++fs",
#endif
			"-o", out, in
		});
	}

	const char* Compiler::getSharedLibraryExtension() noexcept
	{
#if SOUP_WINDOWS
		return "dll";
#else
		return "so";
#endif
	}

	std::string Compiler::compileSharedLibrary(const std::string& in, const std::string& out)
	{
		return os::execute("clang", {
			STD_ARG,
#if !SOUP_WINDOWS
			"-lstdc++",
			"-lstdc++fs",
			"-fPIC",
#endif
			"--shared",
			"-o", out, in
		});
	}
}
