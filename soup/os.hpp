#pragma once

#include "base.hpp"

#include <filesystem>
#include <string>
#include <vector>

#if SOUP_WINDOWS
#include <Windows.h>
#include <Winternl.h>
#endif

namespace soup
{
	class os
	{
	public:
		[[nodiscard]] static std::filesystem::path tempfile(const std::string& ext = {});

		static void escape(std::string& str);
		static std::string execute(std::string program, const std::vector<std::string>& args = {});
	private:
		static std::string executeInner(std::string program, const std::vector<std::string>& args);
	public:

#if SOUP_WINDOWS
		[[nodiscard]] static PEB* getCurrentPeb();

		static void stop();
#endif
	};
}
