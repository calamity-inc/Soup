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
		static std::string makeExecutable(const std::string& in, const std::string& out);
		static std::string makeExecutable(const std::vector<std::string>& objects, const std::string& out);

		// Shared / dynamic link libraries (.dll, .so)
		[[nodiscard]] static const char* getSharedLibraryExtension() noexcept;
		static std::string makeSharedLibrary(const std::string& in, const std::string& out);
	};
}
