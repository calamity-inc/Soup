#pragma once

#include "base.hpp"
#include "fwd.hpp"

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
		[[nodiscard]] static size_t filesize(const std::filesystem::path& path); // returns -1 on error

		[[nodiscard]] static std::filesystem::path tempfile(const std::string& ext = {});
		[[nodiscard]] static std::filesystem::path getProgramData();

		static void escape(std::string& str);
	private:
		static void escapeNoCheck(std::string& str);
	public:
		static std::string execute(std::string program, const std::vector<std::string>& args = {});
		static std::string executeLong(std::string program, const std::vector<std::string>& args = {});
	private:
		static void resolveProgram(std::string& program);
		static std::string executeInner(std::string program, const std::vector<std::string>& args);
	public:

		[[nodiscard]] static UniquePtr<AllocRaiiVirtual> allocateExecutable(const std::string& bytecode);
		[[nodiscard]] static UniquePtr<AllocRaiiVirtual> allocateExecutable(const std::vector<uint8_t>& bytecode);
		[[nodiscard]] static void* virtualAlloc(size_t len, int prot);
		static void virtualFree(void* addr, size_t len);
		static void changeProtection(void* addr, size_t len, int prot);

		[[nodiscard]] static void* createFileMapping(std::filesystem::path path, size_t& out_len);
		static void destroyFileMapping(void* addr, size_t len);

#if SOUP_WINDOWS
		static void simulateKeyPress(Key key);
		static void simulateKeyPress(bool ctrl, bool shift, bool alt, Key key);

		[[nodiscard]] static size_t getMemoryUsage();

		[[nodiscard]] static bool isWine();

		[[nodiscard]] static PEB* getCurrentPeb();

		static void stop();
#endif
	};
}
