#pragma once

#include "base.hpp"

#if SOUP_WINDOWS
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <string>

namespace soup
{
	struct SharedLibrary
	{
#if SOUP_WINDOWS
		using handle_t = HMODULE;
#else
		using handle_t = void*;
#endif

		handle_t handle = nullptr;

		explicit SharedLibrary() = default;
		explicit SharedLibrary(const std::string& path);
		explicit SharedLibrary(const char* path);
		explicit SharedLibrary(SharedLibrary&& b);
		~SharedLibrary();

		void operator=(SharedLibrary&& b);

		[[nodiscard]] bool isLoaded() const noexcept;
		bool load(const std::string& path);
		bool load(const char* path);
		void unload();
		void forget();

		[[nodiscard]] void* getAddress(const char* name) const noexcept;
		[[nodiscard]] void* getAddressMandatory(const char* name) const;
	};
}
