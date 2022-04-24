#pragma once

#include <memory>
#include <string>

#include "base.hpp"

#if SOUP_WINDOWS
#include <Windows.h>

#include "fwd.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	class Process
	{
	public:
		const DWORD id;
		const std::string name;

		Process(DWORD id, std::string&& name);

		[[nodiscard]] static UniquePtr<Process> get(const char* name);
		[[nodiscard]] static UniquePtr<Process> get(DWORD id);

		[[nodiscard]] std::shared_ptr<Module> open(DWORD desired_access = PROCESS_CREATE_THREAD | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | SYNCHRONIZE);
	};
}

#endif
