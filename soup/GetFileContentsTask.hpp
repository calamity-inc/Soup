#pragma once

#include "Task.hpp"

#include "DelayedCtor.hpp"
#include "HttpRequestTask.hpp"
#include "string.hpp"

NAMESPACE_SOUP
{
	struct GetFileContentsTask : public PromiseTask<std::string>
	{
		DelayedCtor<HttpRequestTask> http;

		GetFileContentsTask(const Uri& uri)
		{
			if (uri.isFile())
			{
				result = string::fromFile(uri.getFilePath());
				setWorkDone();
			}
			else if (uri.isHttp())
			{
				http.construct(uri);
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
				if (http->result)
				{
					result = std::move(http->result->body);
				}
				setWorkDone();
			}
		}
	};
}
