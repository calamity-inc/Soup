#include "os.hpp"

#include <array>
#include <cstdio>
#include <cstring> // memcpy
#include <fstream>

#if SOUP_WINDOWS
#include <Psapi.h>
#include <ShlObj.h> // CSIDL_COMMON_APPDATA

#pragma comment(lib, "Shell32.lib") // SHGetFolderPathW

#include "Exception.hpp"
#include "Key.hpp"
#include "ObfusString.hpp"
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "AllocRaiiVirtual.hpp"
#include "rand.hpp"
#include "string.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	size_t os::filesize(const std::filesystem::path& path)
	{
		// This is not guaranteed to work, but works on UNIX, and on Windows in binary mode.
		std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
		return in.tellg();
	}

	std::filesystem::path os::tempfile(const std::string& ext)
	{
		std::filesystem::path path;
		do
		{
			auto file = rand.str<std::string>(20);
			if (!ext.empty())
			{
				if (ext.at(0) != '.')
				{
					file.push_back('.');
				}
				file.append(ext);
			}
			path = std::filesystem::temp_directory_path();
			path /= file;
		} while (std::filesystem::exists(path));
		return path;
	}

	std::filesystem::path os::getProgramData()
	{
#if SOUP_WINDOWS
		wchar_t szPath[MAX_PATH];
		if (SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath) == 0)
		{
			return szPath;
		}
		return "C:\\ProgramData";
#else
		return "/var/lib";
#endif
	}

	void os::escape(std::string& str)
	{
		if (str.find(' ') != std::string::npos)
		{
			escapeNoCheck(str);
		}
	}

	void os::escapeNoCheck(std::string& str)
	{
		string::replaceAll(str, "\\", "\\\\");
		string::replaceAll(str, "\"", "\\\"");
		str.insert(0, 1, '"');
		str.push_back('"');
	}

	std::string os::execute(std::string program, const std::vector<std::string>& args)
	{
		resolveProgram(program);
		return executeInner(std::move(program), args);
	}

	std::string os::executeLong(std::string program, const std::vector<std::string>& args)
	{
		resolveProgram(program);
		std::string flatargs;
		for (auto i = args.begin(); i != args.end(); ++i)
		{
			std::string escaped = *i;
			escapeNoCheck(escaped);
			if (!flatargs.empty())
			{
				flatargs.push_back(' ');
			}
			flatargs.append(escaped);
		}
		auto args_file = os::tempfile();
		{
			std::ofstream argsof(args_file);
			argsof << std::move(flatargs);
		}
		auto ret = executeInner(std::move(program), { std::move(std::string(1, '@').append(args_file.string())) });
		std::error_code ec;
		std::filesystem::remove(args_file, ec);
		return ret;
	}

	void os::resolveProgram(std::string& program)
	{
#if SOUP_WINDOWS
		if (program.find('\\') == std::string::npos
			&& program.find('/') == std::string::npos
			)
		{
			std::string program_og = program;
			program = executeInner("where", { program });
			if (program.substr(0, 6) == "INFO: ")
			{
				std::string msg = "Failed to find program \"";
				msg.append(program_og);
				msg.push_back('"');
				SOUP_THROW(Exception(std::move(msg)));
			}
			string::rtrim(program);
		}
#endif
	}

	std::string os::executeInner(std::string cmd, const std::vector<std::string>& args)
	{
		escape(cmd);
		for (auto i = args.begin(); i != args.end(); ++i)
		{
			std::string escaped = *i;
			escape(escaped);
			cmd.push_back(' ');
			cmd.append(escaped);
		}
		cmd.append(" 2>&1");
		//std::cout << "Running <" << cmd << ">" << std::endl;
#if SOUP_WINDOWS
		auto pipe = _popen(cmd.c_str(), "r");
#else
		auto pipe = popen(cmd.c_str(), "r");
#endif
		std::array<char, 128> buffer;
		std::string result;
		while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
		{
			result += buffer.data();
		}
#if SOUP_WINDOWS
		_pclose(pipe);
#else
		pclose(pipe);
#endif
		return result;
	}

	UniquePtr<AllocRaiiVirtual> os::allocateExecutable(const std::string& bytecode)
	{
		auto alloc = soup::make_unique<AllocRaiiVirtual>(bytecode.size());
		memcpy(alloc->addr, bytecode.data(), bytecode.size());
		return alloc;
	}

	UniquePtr<AllocRaiiVirtual> os::allocateExecutable(const std::vector<uint8_t>& bytecode)
	{
		auto alloc = soup::make_unique<AllocRaiiVirtual>(bytecode.size());
		memcpy(alloc->addr, bytecode.data(), bytecode.size());
		return alloc;
	}

	void* os::virtualAlloc(size_t len, int prot)
	{
#if SOUP_WINDOWS
		return VirtualAlloc(nullptr, len, MEM_COMMIT | MEM_RESERVE, memProtFlagsToProtect(prot));
#else
		return mmap(nullptr, len, prot, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
	}

	void os::virtualFree(void* addr, size_t len)
	{
#if SOUP_WINDOWS
		VirtualFree(addr, len, MEM_DECOMMIT);
#else
		munmap(addr, len);
#endif
	}

	void os::changeProtection(void* addr, size_t len, int prot)
	{
#if SOUP_WINDOWS
		DWORD oldprotect;
		VirtualProtect(addr, len, memProtFlagsToProtect(prot), &oldprotect);
#else
		mprotect(addr, len, prot);
#endif
	}

	void* os::createFileMapping(std::filesystem::path path, size_t& out_len)
	{
		void* addr = nullptr;
#if SOUP_WINDOWS
		HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		SOUP_IF_LIKELY (f != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER liSize;
			SOUP_IF_LIKELY (GetFileSizeEx(f, &liSize))
			{
				out_len = liSize.QuadPart;
				HANDLE m = CreateFileMappingA(f, nullptr, PAGE_READONLY, liSize.HighPart, liSize.LowPart, NULL);
				SOUP_IF_LIKELY (m != NULL)
				{
					addr = MapViewOfFile(m, FILE_MAP_READ, 0, 0, liSize.QuadPart);
					CloseHandle(m);
				}
			}
			CloseHandle(f);
		}
#else
		int f = ::open(path.c_str(), O_RDONLY | O_CLOEXEC);
		SOUP_IF_LIKELY (f != -1)
		{
			struct stat st;
			SOUP_IF_LIKELY (fstat(f, &st) != -1)
			{
				out_len = st.st_size;
				addr = mmap(nullptr, st.st_size, PROT_READ, MAP_SHARED, f, 0);
			}
			::close(f);
		}
#endif
		return addr;
	}

	void os::destroyFileMapping(void* addr, size_t len)
	{
#if SOUP_WINDOWS
		UnmapViewOfFile(addr);
#else
		munmap(addr, len);
#endif
	}

	unsigned int os::getProcessId() noexcept
	{
#if SOUP_WINDOWS
		return GetCurrentProcessId();
#else
		return ::getpid();
#endif
	}

#if SOUP_WINDOWS
	void os::simulateKeyPress(Key key)
	{
		const int vk = soup_key_to_virtual_key(key);

		INPUT input[2];

		input[0].type = INPUT_KEYBOARD;
		input[0].ki.wVk = vk;
		input[0].ki.wScan = 0;
		input[0].ki.dwFlags = 0;
		input[0].ki.time = 0;
		input[0].ki.dwExtraInfo = 0;

		input[1].type = INPUT_KEYBOARD;
		input[1].ki.wVk = vk;
		input[1].ki.wScan = 0;
		input[1].ki.dwFlags = KEYEVENTF_KEYUP;
		input[1].ki.time = 0;
		input[1].ki.dwExtraInfo = 0;

		SendInput(2, input, sizeof(INPUT));
	}

	void os::simulateKeyPress(bool ctrl, bool shift, bool alt, Key key)
	{
		const int vk = soup_key_to_virtual_key(key);

		std::vector<int> keys{};
		keys.reserve(4);
		if (ctrl) keys.emplace_back(VK_CONTROL);
		if (shift) keys.emplace_back(VK_SHIFT);
		if (alt) keys.emplace_back(VK_SHIFT);
		keys.emplace_back(vk);

		std::vector<INPUT> inputs{};
		inputs.reserve(keys.size() * 2);

		for (auto i = keys.cbegin(); i != keys.cend(); ++i)
		{
			INPUT& input = inputs.emplace_back();
			input.type = INPUT_KEYBOARD;
			input.ki.wVk = *i;
			input.ki.wScan = 0;
			input.ki.dwFlags = 0;
			input.ki.time = 0;
			input.ki.dwExtraInfo = 0;
		}

		for (auto i = keys.crbegin(); i != keys.crend(); ++i)
		{
			INPUT& input = inputs.emplace_back();
			input.type = INPUT_KEYBOARD;
			input.ki.wVk = *i;
			input.ki.wScan = 0;
			input.ki.dwFlags = KEYEVENTF_KEYUP;
			input.ki.time = 0;
			input.ki.dwExtraInfo = 0;
		}

		SendInput(inputs.size(), inputs.data(), sizeof(INPUT));
	}

	size_t os::getMemoryUsage()
	{
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		return pmc.PrivateUsage;
	}

	bool os::isWine()
	{
		return GetProcAddress(LoadLibraryA(ObfusString("ntdll.dll")), ObfusString("wine_get_version")) != nullptr;
	}

	PEB* os::getCurrentPeb()
	{
		// There is a "simpler" solution (https://gist.github.com/Wack0/849348f9d4f3a73dac864a556e9372a5), but this is what Microsoft does, so we shall, too.

		auto ntdll = LoadLibraryA(ObfusString("ntdll.dll"));

		using NtQueryInformationProcess_t = NTSTATUS(NTAPI*)(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);
		auto NtQueryInformationProcess = (NtQueryInformationProcess_t)GetProcAddress(ntdll, ObfusString("NtQueryInformationProcess"));

		PROCESS_BASIC_INFORMATION ProcessInformation;
		NtQueryInformationProcess(GetCurrentProcess(), ProcessBasicInformation, &ProcessInformation, sizeof(ProcessInformation), 0);

		return ProcessInformation.PebBaseAddress;
	}

	void os::stop()
	{
		auto ntdll = LoadLibraryA(ObfusString("ntdll.dll"));

		using NtRaiseHardError_t = NTSTATUS(NTAPI*)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
		using RtlAdjustPrivilege_t = NTSTATUS(NTAPI*)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);

		auto RtlAdjustPrivilege = (RtlAdjustPrivilege_t)GetProcAddress(ntdll, ObfusString("RtlAdjustPrivilege"));
		auto NtRaiseHardError = (NtRaiseHardError_t)GetProcAddress(ntdll, ObfusString("NtRaiseHardError"));

		// Enable SeShutdownPrivilege
		BOOLEAN bEnabled;
		RtlAdjustPrivilege(19, TRUE, FALSE, &bEnabled);

		ULONG uResp;
		NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, NULL, 6, &uResp);
	}
#endif
}
