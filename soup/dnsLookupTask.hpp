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

	struct dnsCachedResultTask : public dnsLookupTask
	{
		static UniquePtr<dnsCachedResultTask> make(std::vector<UniquePtr<dnsRecord>>&& res)
		{
			auto task = soup::make_unique<dnsCachedResultTask>();
			task->res = std::move(res);
			task->setWorkDone();
			return task;
		}

		void onTick() final
		{
			SOUP_ASSERT_UNREACHABLE;
		}
	};
}
