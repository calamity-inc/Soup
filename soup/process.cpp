#include "process.hpp"

#if SOUP_WINDOWS

#include <TlHelp32.h>

#include "module.hpp"
#include "handle_raii.hpp"

namespace soup
{
	process::process(DWORD id, std::string&& name)
		: id(id), name(std::move(name))
	{
	}

	std::unique_ptr<process> process::get(const char* name)
	{
		handle_raii hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnap)
		{
			PROCESSENTRY32 entry;
			entry.dwSize = sizeof(entry);
			if (Process32First(hSnap, &entry))
			{
				do
				{
					if (strcmp(entry.szExeFile, name) == 0)
					{
						return std::make_unique<process>(entry.th32ProcessID, entry.szExeFile);
					}
				} while (Process32Next(hSnap, &entry));
			}
		}
		return {};
	}

	std::unique_ptr<process> process::get(DWORD id)
	{
		handle_raii hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnap)
		{
			PROCESSENTRY32 entry;
			entry.dwSize = sizeof(entry);
			if (Process32First(hSnap, &entry))
			{
				do
				{
					if (entry.th32ProcessID == id)
					{
						return std::make_unique<process>(entry.th32ProcessID, entry.szExeFile);
					}
				} while (Process32Next(hSnap, &entry));
			}
		}
		return {};
	}

	std::shared_ptr<module> process::open(DWORD desired_access)
	{
		handle_raii hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, id);
		if (hSnap)
		{
			MODULEENTRY32 entry;
			entry.dwSize = sizeof(entry);
			if (Module32First(hSnap, &entry))
			{
				do
				{
					if (this->name == entry.szModule)
					{
						return std::make_unique<module>(std::make_unique<handle_raii>(OpenProcess(desired_access, FALSE, id)), range(entry.modBaseAddr, entry.modBaseSize));
					}
				} while (Module32Next(hSnap, &entry));
			}
		}
		return {};
	}
}

#endif
