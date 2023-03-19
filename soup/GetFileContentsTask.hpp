#pragma once

#include "Task.hpp"

#include "DelayedCtor.hpp"
#include "HttpRequestTask.hpp"
#include "string.hpp"

namespace soup
{
	struct GetFileContentsTask : public Task
	{
		std::string contents;
		DelayedCtor<HttpRequestTask> http;

		GetFileContentsTask(Scheduler* sched, const Uri& uri)
		{
			if (uri.isFile())
			{
				contents = string::fromFile(uri.getFilePath());
				setWorkDone();
			}
			else if (uri.isHttp())
			{
				http.construct(sched, uri);
			}
			else
			{
				throw 0;
			}
		}

		void onTick() final
		{
			if (http->tickUntilDone())
			{
				if (http->res)
				{
					contents = std::move(http->res->body);
				}
				setWorkDone();
			}
		}
	};
}
