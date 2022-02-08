#pragma once

#include <memory>
#include <string>

#include "platform.hpp"
#if SOUP_PLATFORM_WINDOWS

#include <Windows.h>

#include "fwddecl.hpp"

namespace soup
{
	class process
	{
	public:
		const DWORD id;
		const std::string name;

		process(DWORD id, std::string&& name);

		[[nodiscard]] static std::unique_ptr<process> get(const char* name);
		[[nodiscard]] static std::unique_ptr<process> get(DWORD id);
		
		[[nodiscard]] std::shared_ptr<module> open(DWORD desired_access = PROCESS_CREATE_THREAD | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | SYNCHRONIZE);
	};
}

#endif
