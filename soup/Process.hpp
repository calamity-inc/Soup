#pragma once

#include "base.hpp"
#include "fwd.hpp"
#include "type.hpp"
#include "UniquePtr.hpp"

#include <vector>

#if SOUP_WINDOWS
	#include <memory>
	#include <string>

	#include <Windows.h>
#endif

NAMESPACE_SOUP
{
	class Process
	{
	public:
		const pid_t id;
#if SOUP_WINDOWS
		const std::string name;
#endif

#if SOUP_WINDOWS
		Process(pid_t id, std::string&& name)
			: id(id), name(std::move(name))
		{
		}
#else
		Process(pid_t id)
			: id(id)
		{
		}
#endif

		[[nodiscard]] static UniquePtr<Process> get(pid_t id);
#if SOUP_WINDOWS
		[[nodiscard]] static UniquePtr<Process> get(const char* name);
		[[nodiscard]] static std::vector<UniquePtr<Process>> getAll();

		[[nodiscard]] std::shared_ptr<Module> open(DWORD desired_access = PROCESS_CREATE_THREAD | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | SYNCHRONIZE) const;
#endif

		[[nodiscard]] std::vector<Range> getAllocations() const;
	};
}
