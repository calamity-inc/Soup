#pragma once

#include "Task.hpp"

#include <vector>

#include "dns_records.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	struct dnsLookupTask : public Task
	{
		std::vector<UniquePtr<dnsRecord>> res;

		using Task::Task;
	};
}
