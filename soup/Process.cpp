#include "Process.hpp"

#if SOUP_WINDOWS
	#include <TlHelp32.h>

	#include "HandleRaii.hpp"
	#include "Module.hpp"
#else
	#include "FileReader.hpp"
	#include "Range.hpp"
	#include "Regex.hpp"
	#include "string.hpp"
#endif

NAMESPACE_SOUP
{
	UniquePtr<Process> Process::get(pid_t id)
	{
#if SOUP_WINDOWS
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
#else
		return soup::make_unique<Process>(id);
#endif
	}

#if SOUP_WINDOWS
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

	std::shared_ptr<Module> Process::open(DWORD desired_access) const
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
#endif

	std::vector<Range> Process::getAllocations() const
	{
#if SOUP_WINDOWS
		auto m = open();
		return m->getAllocations();
#else
		std::vector<Range> res{};
		Regex r(R"(^(?'start'[0-9A-Fa-f]+)-(?'end'[0-9A-Fa-f]+) +(?'prots'[a-z\-]+) +[^ ]+ +[^ ]+ +[^ ]+ +(?'mappedfile'.*)$)");
		FileReader fr("/proc/" + std::to_string(this->id) + "/maps");
		for (std::string line; fr.getLine(line); )
		{
			auto m = r.match(line);
			if (m.findGroupByName("prots")->begin[0] != 'r')
			{
				continue;
			}
			if (auto mappedfile = m.findGroupByName("mappedfile"))
			{
				if (mappedfile->length() != 0)
				{
					continue;
				}
			}
			auto start = string::hexToInt<uintptr_t>(m.findGroupByName("start")->toString()).value();
			auto end = string::hexToInt<uintptr_t>(m.findGroupByName("end")->toString()).value();
			res.emplace_back(start, end - start);
		}
		return res;
#endif
	}
}
