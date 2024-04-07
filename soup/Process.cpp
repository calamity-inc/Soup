#include "Process.hpp"

#if SOUP_WINDOWS

#include <TlHelp32.h>

#include "HandleRaii.hpp"
#include "Module.hpp"

NAMESPACE_SOUP
{
	Process::Process(DWORD id, std::string&& name)
		: id(id), name(std::move(name))
	{
	}

	UniquePtr<Process> Process::get(const char* name)
	{
		HandleRaii hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
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
						return soup::make_unique<Process>(entry.th32ProcessID, entry.szExeFile);
					}
				} while (Process32Next(hSnap, &entry));
			}
		}
		return {};
	}

	UniquePtr<Process> Process::get(DWORD id)
	{
		HandleRaii hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
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
						return soup::make_unique<Process>(entry.th32ProcessID, entry.szExeFile);
					}
				} while (Process32Next(hSnap, &entry));
			}
		}
		return {};
	}

	std::vector<UniquePtr<Process>> Process::getAll()
	{
		std::vector<UniquePtr<Process>> res{};
		HandleRaii hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnap)
		{
			PROCESSENTRY32 entry;
			entry.dwSize = sizeof(entry);
			if (Process32First(hSnap, &entry))
			{
				do
				{
					res.emplace_back(soup::make_unique<Process>(entry.th32ProcessID, entry.szExeFile));
				} while (Process32Next(hSnap, &entry));
			}
		}
		return res;
	}

	std::shared_ptr<Module> Process::open(DWORD desired_access)
	{
		HandleRaii hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, id);
		if (hSnap)
		{
			MODULEENTRY32 entry;
			entry.dwSize = sizeof(entry);
			if (Module32First(hSnap, &entry))
			{
				do
				{
					if (this->id == entry.th32ProcessID)
					{
						return std::make_shared<Module>(make_unique<HandleRaii>(OpenProcess(desired_access, FALSE, id)), Range(entry.modBaseAddr, entry.modBaseSize));
					}
				} while (Module32Next(hSnap, &entry));
			}
		}
		return {};
	}
}

#endif
