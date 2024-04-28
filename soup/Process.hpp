#pragma once

#include "base.hpp"
#if SOUP_WINDOWS
#include <Windows.h>

#include <memory>
#include <string>
#include <vector>

#include "fwd.hpp"
#include "UniquePtr.hpp"

NAMESPACE_SOUP
{
	class Process
	{
	public:
		const DWORD id;
		const std::string name;

		Process(DWORD id, std::string&& name);

		[[nodiscard]] static UniquePtr<Process> get(const char* name);
		[[nodiscard]] static UniquePtr<Process> get(DWORD id);
		[[nodiscard]] static std::vector<UniquePtr<Process>> getAll();

		[[nodiscard]] std::shared_ptr<Module> open(DWORD desired_access = PROCESS_CREATE_THREAD | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | SYNCHRONIZE) const;

		[[nodiscard]] std::vector<Range> getAllocations() const;
	};
}

#endif
