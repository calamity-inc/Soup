#include "Compiler.hpp"

#include "base.hpp"

#include "os.hpp"

#if SOUP_WINDOWS
#define STD_ARG "-std=c++20"
#else
// Debian's at clang 11 right now, which does support C++20, but not enough to compile Soup without modifications.
#define STD_ARG "-std=c++17"
#endif

#if SOUP_WINDOWS
#define CLANG_DEFAULT_ARGS STD_ARG, "-D", "_CRT_SECURE_NO_WARNINGS"
#else
#define CLANG_DEFAULT_ARGS STD_ARG, "-lstdc++", "-lstdc++fs"
#endif

namespace soup
{
	std::string Compiler::makeObject(const std::string& in, const std::string& out)
	{
		return os::execute("clang", {
			CLANG_DEFAULT_ARGS,
			"-o", out, "-c", in
		});
	}

	const char* Compiler::getExecutableExtension() noexcept
	{
#if SOUP_WINDOWS
		return ".exe";
#else
		return "";
#endif
	}

	std::string Compiler::makeExecutable(const std::string& in, const std::string& out)
	{
		return os::execute("clang", {
			CLANG_DEFAULT_ARGS,
			"-o", out, in
		});
	}

	std::string Compiler::makeExecutable(const std::vector<std::string>& objects, const std::string& out)
	{
		std::vector<std::string> args = {
			CLANG_DEFAULT_ARGS,
			"-o", out
		};
		args.insert(args.end(), objects.begin(), objects.end());
		return os::execute("clang", std::move(args));
	}

	const char* Compiler::getStaticLibraryExtension() noexcept
	{
#if SOUP_WINDOWS
		return ".lib";
#else
		return ".a";
#endif
	}

	std::string Compiler::makeStaticLibrary(const std::vector<std::string>& objects, const std::string& out)
	{
		std::vector<std::string> args = { "rc", out };
		args.insert(args.end(), objects.begin(), objects.end());
#if SOUP_WINDOWS
		return os::execute("llvm-ar", std::move(args));
#else
		return os::execute("ar", std::move(args));
#endif
	}

	const char* Compiler::getSharedLibraryExtension() noexcept
	{
#if SOUP_WINDOWS
		return ".dll";
#else
		return ".so";
#endif
	}

	std::string Compiler::makeSharedLibrary(const std::string& in, const std::string& out)
	{
		return os::execute("clang", {
			CLANG_DEFAULT_ARGS,
#if !SOUP_WINDOWS
			"-fPIC",
#endif
			"--shared",
			"-o", out, in
		});
	}
}
