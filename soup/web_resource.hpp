#pragma once

#include "base.hpp"

#include "callback.hpp"
#include <string>

namespace soup
{
	struct web_resource
	{
		std::string host;
		std::string path;

		bool has_data = false;
		std::string data{};

		web_resource(std::string&& host, std::string&& path);

#if SOUP_WASM
		void download(callback<void(web_resource&)>&& cb = {});
#else
		void download(callback<void(web_resource&)>&& cb); // blocking
		void download(); // blocking
#endif
	};
}
