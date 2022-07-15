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
	std::string Compiler::makeObject(const std::string& in, const std::string& out)
	{
		return os::execute("clang", {
			STD_ARG,
#if !SOUP_WINDOWS
			"-lstdc++",
			"-lstdc++fs",
#endif
			"-o", out, "-c", in
			});
	}

	std::string Compiler::makeExecutable(const std::string& in, const std::string& out)
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

	std::string Compiler::makeExecutable(const std::vector<std::string>& objects, const std::string& out)
	{
		std::vector<std::string> args = {
			STD_ARG,
#if !SOUP_WINDOWS
			"-lstdc++",
			"-lstdc++fs",
#endif
			"-o", out
		};
		args.insert(args.end(), objects.begin(), objects.end());
		return os::execute("clang", std::move(args));
	}

	const char* Compiler::getSharedLibraryExtension() noexcept
	{
#if SOUP_WINDOWS
		return "dll";
#else
		return "so";
#endif
	}

	std::string Compiler::makeSharedLibrary(const std::string& in, const std::string& out)
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
