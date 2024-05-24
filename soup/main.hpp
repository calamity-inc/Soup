#pragma once

#include "base.hpp"

#include <string>
#include <vector>

#if SOUP_WINDOWS
#include <windows.h>
#endif

#if SOUP_WINDOWS
// make sure you have the /SUBSYSTEM linker option set to WINDOWS
#define SOUP_MAIN_GUI(entrypoint) int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) { return ::soup::main_impl::windows_gui(entrypoint, lpCmdLine); }
#define SOUP_MAIN_CLI(entrypoint) int __cdecl wmain(int argc, wchar_t** argv) { return ::soup::main_impl::windows(argc, argv, entrypoint, true); }
#else
#define SOUP_MAIN_CLI(entrypoint) int main(int argc, const char** argv) { return ::soup::main_impl::cli(argc, argv, entrypoint); }
#define SOUP_MAIN_GUI(entrypoint) SOUP_MAIN_CLI(entrypoint)
#endif


NAMESPACE_SOUP
{
	struct main_impl
	{
		using entrypoint_t = int(*)(std::vector<std::string>&& args, bool console);

#if SOUP_WINDOWS
		static int windows_gui(entrypoint_t entrypoint, LPWSTR lpCmdLine);
		static int windows(int argc, wchar_t** argv, entrypoint_t entrypoint, bool console);
#else
		static int cli(int argc, const char** argv, entrypoint_t entrypoint);
#endif
	};
}
