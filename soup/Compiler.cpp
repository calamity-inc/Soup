#include "Compiler.hpp"

#include "base.hpp"

#include "os.hpp"

namespace soup
{
	Compiler::Compiler()
#if SOUP_WINDOWS
		: lang("c++20")
#else
		// Debian's at clang 11 right now, which does support C++20, but not enough to compile Soup without modifications.
		: lang("c++17")
#endif
	{
	}

	std::vector<std::string> Compiler::getArgs() const
	{
		std::vector<std::string> args{
#if SOUP_WINDOWS
			"-D", "_CRT_SECURE_NO_WARNINGS",
#else
			"-lm", "-lstdc++", "-lstdc++fs", "-pthreads", "-Wno-unused-command-line-argument",
#endif
			"-std="
		};
		args.back().append(lang);
		if (!rtti)
		{
			args.emplace_back("-fno-rtti");
		}
		args.insert(args.end(), extra_args.begin(), extra_args.end());
		return args;
	}

	std::vector<std::string> Compiler::getLinkerArgs() const
	{
		auto args = getArgs();
		args.insert(args.end(), extra_linker_args.begin(), extra_linker_args.end());
		return args;
	}

	std::string Compiler::makeObject(const std::string& in, const std::string& out) const
	{
		auto args = getArgs();
		args.emplace_back("-o");
		args.emplace_back(out);
		args.emplace_back("-c");
		args.emplace_back(in);
		return os::execute("clang", std::move(args));
	}

	const char* Compiler::getExecutableExtension() noexcept
	{
#if SOUP_WINDOWS
		return ".exe";
#else
		return "";
#endif
	}

	std::string Compiler::makeExecutable(const std::string& in, const std::string& out) const
	{
		auto args = getLinkerArgs();
		args.emplace_back("-o");
		args.emplace_back(out);
		args.emplace_back(in);
		return os::execute("clang", std::move(args));
	}

	std::string Compiler::makeExecutable(const std::vector<std::string>& objects, const std::string& out) const
	{
		auto args = getLinkerArgs();
		args.emplace_back("-o");
		args.emplace_back(out);
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

	std::string Compiler::makeStaticLibrary(const std::vector<std::string>& objects, const std::string& out) const
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

	std::string Compiler::makeSharedLibrary(const std::string& in, const std::string& out) const
	{
		auto args = getLinkerArgs();
#if !SOUP_WINDOWS
		args.emplace_back("-fPIC");
#endif
		args.emplace_back("--shared");
		args.emplace_back("-o");
		args.emplace_back(out);
		args.emplace_back(in);
		return os::execute("clang", std::move(args));
	}
}
