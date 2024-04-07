#include "main.hpp"

#if SOUP_WINDOWS
#include <shellapi.h>
#pragma comment(lib, "shell32")
#endif

#include "unicode.hpp"

NAMESPACE_SOUP
{
#if SOUP_WINDOWS
	int main_impl::windows_gui(entrypoint_t entrypoint, LPWSTR lpCmdLine)
	{
		bool console = false;
		if (AttachConsole(ATTACH_PARENT_PROCESS))
		{
			console = true;

			FILE* f;
			freopen_s(&f, "CONIN$", "r", stdin);
			freopen_s(&f, "CONOUT$", "w", stderr);
			freopen_s(&f, "CONOUT$", "w", stdout);
		}

		int argc;
		wchar_t** argv = CommandLineToArgvW(lpCmdLine, &argc);

		return windows(argc, argv, entrypoint, console);
	}

	int main_impl::windows(int argc, wchar_t** argv, entrypoint_t entrypoint, bool console)
	{
		std::vector<std::string> args{};
		for (int i = 0; i != argc; ++i)
		{
			args.emplace_back(unicode::utf16_to_utf8<std::wstring>(argv[i]));
		}
		return entrypoint(std::move(args), console);
	}
#else
	int main_impl::cli(int argc, const char** argv, entrypoint_t entrypoint)
	{
		std::vector<std::string> args{};
		for (int i = 0; i != argc; ++i)
		{
			args.emplace_back(argv[i]);
		}
		return entrypoint(std::move(args), true);
	}
#endif
}
