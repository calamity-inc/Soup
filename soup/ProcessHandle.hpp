#pragma once

#include "base.hpp"

#if SOUP_WINDOWS
#include "Module.hpp"
#else
#include "type.hpp"
#endif

#include "fwd.hpp"
#include <vector>

NAMESPACE_SOUP
{
	class ProcessHandle
#if SOUP_WINDOWS
		: public Module
#endif
	{
#if SOUP_WINDOWS
	public:
		using Module::Module;
#else
	private:
		const pid_t pid;
	public:
		ProcessHandle(const pid_t pid)
			: pid(pid)
		{
		}
#endif

		[[nodiscard]] std::vector<Range> getAllocations() const;
	};
}
